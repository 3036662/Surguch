import QtQuick

Rectangle {
    id: disappearingHint

    width: hintText.width + 40
    height: hintText.height + 20
    color: "#ff3333"
    radius: 8
    opacity: 0
    visible: opacity > 0

    Behavior on opacity {
        NumberAnimation {
            duration: 200
        }
    }

    Text {
        id: hintText
        anchors.centerIn: parent
        color: "white"
        font.pixelSize: 16
    }

    Timer {
        id: hideTimer
        interval: 1000
        onTriggered: disappearingHint.opacity = 0
    }

    function showHint(message, duration) {
        hintText.text = message
        hideTimer.interval = duration
        disappearingHint.opacity = 1
        hideTimer.restart()
    }
}
