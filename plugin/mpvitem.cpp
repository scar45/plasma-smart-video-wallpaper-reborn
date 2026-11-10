/*
    SPDX-FileCopyrightText: 2023 George Florea Bănuș <georgefb899@gmail.com>
    SPDX-FileCopyrightText: 2026 George Merlocco <george@merloc.co>

    SPDX-License-Identifier: MIT
*/

#include "mpvitem.h"

#include <MpvController>

using namespace Qt::StringLiterals;

MpvItem::MpvItem(QQuickItem *parent)
    : MpvAbstractItem(parent)
{
    setProperty(u"hwdec"_s, u"auto-safe"_s);
    setProperty(u"pause"_s, true);
    applyLoops();
    applyFillMode();

    observeProperty(u"time-pos"_s, MPV_FORMAT_DOUBLE);
    observeProperty(u"duration"_s, MPV_FORMAT_DOUBLE);
    observeProperty(u"pause"_s, MPV_FORMAT_FLAG);

    connect(mpvController(), &MpvController::propertyChanged, this, &MpvItem::onPropertyChanged, Qt::QueuedConnection);
    connect(mpvController(), &MpvController::fileLoaded, this, &MpvItem::onFileLoaded, Qt::QueuedConnection);
    connect(mpvController(), &MpvController::endFile, this, &MpvItem::onEndFile, Qt::QueuedConnection);
    connect(
        mpvController(),
        &MpvController::playbackRestart,
        this,
        [this] {
            if (m_mediaStatus == QMediaPlayer::LoadedMedia) {
                setMediaStatus(QMediaPlayer::BufferedMedia);
            }
        },
        Qt::QueuedConnection);

    // libmpv needs the render context before loading, otherwise video output is disabled for the file
    connect(
        this,
        &MpvAbstractItem::ready,
        this,
        [this] {
            if (!m_rendererReady) {
                m_rendererReady = true;
                load();
            }
        },
        Qt::QueuedConnection);
}

QUrl MpvItem::source() const
{
    return m_source;
}

void MpvItem::setSource(const QUrl &source)
{
    if (m_source == source) {
        return;
    }
    m_source = source;
    Q_EMIT sourceChanged();
    load();
}

bool MpvItem::autoPlay() const
{
    return m_autoPlay;
}

void MpvItem::setAutoPlay(bool autoPlay)
{
    if (m_autoPlay == autoPlay) {
        return;
    }
    m_autoPlay = autoPlay;
    Q_EMIT autoPlayChanged();
}

int MpvItem::loops() const
{
    return m_loops;
}

void MpvItem::setLoops(int loops)
{
    if (m_loops == loops) {
        return;
    }
    m_loops = loops;
    applyLoops();
    Q_EMIT loopsChanged();
}

qreal MpvItem::playbackRate() const
{
    return m_playbackRate;
}

void MpvItem::setPlaybackRate(qreal rate)
{
    if (qFuzzyCompare(m_playbackRate, rate)) {
        return;
    }
    m_playbackRate = rate;
    setProperty(u"speed"_s, rate);
    Q_EMIT playbackRateChanged();
}

qreal MpvItem::volume() const
{
    return m_volume;
}

void MpvItem::setVolume(qreal volume)
{
    if (qFuzzyCompare(m_volume, volume)) {
        return;
    }
    m_volume = volume;
    setProperty(u"volume"_s, volume * 100.0);
    Q_EMIT volumeChanged();
}

bool MpvItem::muted() const
{
    return m_muted;
}

void MpvItem::setMuted(bool muted)
{
    if (m_muted == muted) {
        return;
    }
    m_muted = muted;
    setProperty(u"mute"_s, muted);
    Q_EMIT mutedChanged();
}

bool MpvItem::audioEnabled() const
{
    return m_audioEnabled;
}

void MpvItem::setAudioEnabled(bool enabled)
{
    if (m_audioEnabled == enabled) {
        return;
    }
    m_audioEnabled = enabled;
    setProperty(u"aid"_s, enabled ? u"auto"_s : u"no"_s);
    Q_EMIT audioEnabledChanged();
}

Qt::AspectRatioMode MpvItem::fillMode() const
{
    return m_fillMode;
}

void MpvItem::setFillMode(Qt::AspectRatioMode mode)
{
    if (m_fillMode == mode) {
        return;
    }
    m_fillMode = mode;
    applyFillMode();
    Q_EMIT fillModeChanged();
}

qint64 MpvItem::position() const
{
    return m_position;
}

qint64 MpvItem::duration() const
{
    return m_duration;
}

bool MpvItem::playing() const
{
    return m_loaded && !m_paused;
}

bool MpvItem::seekable() const
{
    return m_loaded;
}

QMediaPlayer::MediaStatus MpvItem::mediaStatus() const
{
    return m_mediaStatus;
}

void MpvItem::play()
{
    setProperty(u"pause"_s, false);
}

void MpvItem::pause()
{
    setProperty(u"pause"_s, true);
}

void MpvItem::stop()
{
    pause();
    setPosition(0);
}

void MpvItem::setPosition(qint64 position)
{
    if (m_loaded) {
        seek(position);
    } else {
        m_pendingPosition = position;
    }
}

void MpvItem::load()
{
    setLoaded(false);
    m_pendingPosition = -1;

    if (m_source.isEmpty()) {
        command({u"stop"_s});
        setMediaStatus(QMediaPlayer::NoMedia);
        return;
    }

    setMediaStatus(QMediaPlayer::LoadingMedia);
    if (!m_rendererReady) {
        return;
    }

    setProperty(u"pause"_s, !m_autoPlay);
    command({u"loadfile"_s, m_source.toString(QUrl::PreferLocalFile)});
}

void MpvItem::seek(qint64 position)
{
    command({u"seek"_s, QString::number(position / 1000.0), u"absolute"_s});
}

void MpvItem::applyLoops()
{
    if (m_loops == QMediaPlayer::Infinite) {
        setProperty(u"loop-file"_s, u"inf"_s);
    } else if (m_loops > 1) {
        setProperty(u"loop-file"_s, QString::number(m_loops - 1));
    } else {
        setProperty(u"loop-file"_s, u"no"_s);
    }
}

void MpvItem::applyFillMode()
{
    setProperty(u"keepaspect"_s, m_fillMode != Qt::IgnoreAspectRatio);
    setProperty(u"panscan"_s, m_fillMode == Qt::KeepAspectRatioByExpanding ? 1.0 : 0.0);
}

void MpvItem::setMediaStatus(QMediaPlayer::MediaStatus status)
{
    if (m_mediaStatus == status) {
        return;
    }
    m_mediaStatus = status;
    Q_EMIT mediaStatusChanged();
}

void MpvItem::setLoaded(bool loaded)
{
    if (m_loaded == loaded) {
        return;
    }
    m_loaded = loaded;
    Q_EMIT seekableChanged();
    Q_EMIT playingChanged();
}

void MpvItem::onPropertyChanged(const QString &property, const QVariant &value)
{
    if (property == u"time-pos") {
        const qint64 position = value.toDouble() * 1000;
        if (m_position != position) {
            m_position = position;
            Q_EMIT positionChanged();
        }
    } else if (property == u"duration") {
        const qint64 duration = value.toDouble() * 1000;
        if (m_duration != duration) {
            m_duration = duration;
            Q_EMIT durationChanged();
        }
    } else if (property == u"pause") {
        const bool paused = value.toBool();
        if (m_paused != paused) {
            m_paused = paused;
            Q_EMIT playingChanged();
        }
    }
}

void MpvItem::onFileLoaded()
{
    // the observed duration may arrive after this event, consumers expect it to be known once loaded
    const qint64 duration = getProperty(u"duration"_s).toDouble() * 1000;
    if (m_duration != duration) {
        m_duration = duration;
        Q_EMIT durationChanged();
    }

    setLoaded(true);
    if (m_pendingPosition >= 0) {
        seek(m_pendingPosition);
        m_pendingPosition = -1;
    }
    setMediaStatus(QMediaPlayer::LoadedMedia);
}

void MpvItem::onEndFile(const QString &reason)
{
    if (reason == u"eof") {
        setLoaded(false);
        setMediaStatus(QMediaPlayer::EndOfMedia);
    } else if (reason == u"error") {
        setLoaded(false);
        setMediaStatus(QMediaPlayer::InvalidMedia);
    }
}

#include "moc_mpvitem.cpp"
