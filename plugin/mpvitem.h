/*
    SPDX-FileCopyrightText: 2023 George Florea Bănuș <georgefb899@gmail.com>
    SPDX-FileCopyrightText: 2026 George Merlocco <george@merloc.co>

    SPDX-License-Identifier: MIT
*/

#pragma once

#include <MpvAbstractItem>
#include <QMediaPlayer>
#include <QUrl>
#include <qqmlintegration.h>

// MediaPlayer-compatible video item backed by libmpv
class MpvItem : public MpvAbstractItem
{
    Q_OBJECT
    QML_ELEMENT

    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(bool autoPlay READ autoPlay WRITE setAutoPlay NOTIFY autoPlayChanged)
    Q_PROPERTY(int loops READ loops WRITE setLoops NOTIFY loopsChanged)
    Q_PROPERTY(qreal playbackRate READ playbackRate WRITE setPlaybackRate NOTIFY playbackRateChanged)
    Q_PROPERTY(qreal volume READ volume WRITE setVolume NOTIFY volumeChanged)
    Q_PROPERTY(bool muted READ muted WRITE setMuted NOTIFY mutedChanged)
    Q_PROPERTY(bool audioEnabled READ audioEnabled WRITE setAudioEnabled NOTIFY audioEnabledChanged)
    Q_PROPERTY(Qt::AspectRatioMode fillMode READ fillMode WRITE setFillMode NOTIFY fillModeChanged)
    Q_PROPERTY(qint64 position READ position NOTIFY positionChanged)
    Q_PROPERTY(qint64 duration READ duration NOTIFY durationChanged)
    Q_PROPERTY(bool playing READ playing NOTIFY playingChanged)
    Q_PROPERTY(bool seekable READ seekable NOTIFY seekableChanged)
    Q_PROPERTY(QMediaPlayer::MediaStatus mediaStatus READ mediaStatus NOTIFY mediaStatusChanged)

public:
    explicit MpvItem(QQuickItem *parent = nullptr);

    QUrl source() const;
    void setSource(const QUrl &source);
    bool autoPlay() const;
    void setAutoPlay(bool autoPlay);
    int loops() const;
    void setLoops(int loops);
    qreal playbackRate() const;
    void setPlaybackRate(qreal rate);
    qreal volume() const;
    void setVolume(qreal volume);
    bool muted() const;
    void setMuted(bool muted);
    bool audioEnabled() const;
    void setAudioEnabled(bool enabled);
    Qt::AspectRatioMode fillMode() const;
    void setFillMode(Qt::AspectRatioMode mode);
    qint64 position() const;
    qint64 duration() const;
    bool playing() const;
    bool seekable() const;
    QMediaPlayer::MediaStatus mediaStatus() const;

    Q_INVOKABLE void play();
    Q_INVOKABLE void pause();
    Q_INVOKABLE void stop();
    Q_INVOKABLE void setPosition(qint64 position);

Q_SIGNALS:
    void sourceChanged();
    void autoPlayChanged();
    void loopsChanged();
    void playbackRateChanged();
    void volumeChanged();
    void mutedChanged();
    void audioEnabledChanged();
    void fillModeChanged();
    void positionChanged();
    void durationChanged();
    void playingChanged();
    void seekableChanged();
    void mediaStatusChanged();

private:
    void load();
    void seek(qint64 position);
    void applyLoops();
    void applyFillMode();
    void setMediaStatus(QMediaPlayer::MediaStatus status);
    void setLoaded(bool loaded);
    void onPropertyChanged(const QString &property, const QVariant &value);
    void onFileLoaded();
    void onEndFile(const QString &reason);

    QUrl m_source;
    bool m_autoPlay{false};
    int m_loops{1};
    qreal m_playbackRate{1.0};
    qreal m_volume{1.0};
    bool m_muted{false};
    bool m_audioEnabled{true};
    Qt::AspectRatioMode m_fillMode{Qt::KeepAspectRatioByExpanding};
    qint64 m_position{0};
    qint64 m_duration{0};
    bool m_paused{true};
    bool m_loaded{false};
    bool m_rendererReady{false};
    qint64 m_pendingPosition{-1};
    QMediaPlayer::MediaStatus m_mediaStatus{QMediaPlayer::NoMedia};
};
