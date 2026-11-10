pragma ComponentBehavior: Bound
import QtQuick
import QtMultimedia
import com.github.luisbocanegra.svwr

// Exposes the MediaPlayer API backed by either Qt Multimedia or MpvQt
Item {
    id: root
    property bool useMpvQt: false
    property url source
    property real playbackRate: 1
    property int loops: MediaPlayer.Once
    property bool autoPlay: false
    property VideoOutput videoOutput
    property AudioOutput audioOutput
    property bool disableAudioOutput: false

    readonly property int position: loader.item?.position ?? 0
    readonly property int duration: loader.item?.duration ?? 0
    readonly property int mediaStatus: loader.item?.mediaStatus ?? MediaPlayer.NoMedia
    readonly property bool playing: loader.item?.playing ?? false
    readonly property bool seekable: loader.item?.seekable ?? false

    function play() {
        loader.item?.play();
    }
    function pause() {
        loader.item?.pause();
    }
    function stop() {
        loader.item?.stop();
    }
    function setPosition(position) {
        loader.item?.setPosition(position);
    }

    Loader {
        id: loader
        anchors.fill: parent
        sourceComponent: root.useMpvQt ? mpvPlayer : qtPlayer
    }

    Component {
        id: qtPlayer
        MediaPlayer {
            videoOutput: root.videoOutput
            audioOutput: root.disableAudioOutput ? null : root.audioOutput
            source: root.source
            playbackRate: root.playbackRate
            loops: root.loops
            autoPlay: root.autoPlay
        }
    }

    Component {
        id: mpvPlayer
        MpvItem {
            source: root.source
            playbackRate: root.playbackRate
            loops: root.loops
            autoPlay: root.autoPlay
            fillMode: root.videoOutput.fillMode
            volume: root.audioOutput.volume
            muted: root.audioOutput.muted
            audioEnabled: !root.disableAudioOutput
        }
    }
}
