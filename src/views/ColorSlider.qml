import QtQuick
import QtQuick.Controls

Slider {
    id: control

    property color back_color

    snapMode: Slider.SnapOnRelease
    from: 0
    to: 255
    stepSize: 1
    background: Rectangle {
        x: control.leftPadding
        y: control.topPadding + control.availableHeight / 2 - height / 2
        implicitWidth: 200
        implicitHeight: 6
        width: control.availableWidth
        height: implicitHeight
        radius: 2
        color: control.back_color

        Rectangle {
            width: control.visualPosition * parent.width
            height: parent.height
            color: control.back_color
            radius: 2
        }
    }

    handle: Rectangle {
        x: control.leftPadding + control.visualPosition * (control.availableWidth - width)
        y: control.topPadding + control.availableHeight / 2 - height / 2
        implicitWidth: 15
        implicitHeight: 15
        radius: 13
    }
}
