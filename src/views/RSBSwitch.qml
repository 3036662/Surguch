import QtQuick
import QtQuick.Controls

Switch {
    id: control

    width: parent.width

    indicator: Rectangle {
        implicitWidth: 35
        implicitHeight: 20
        x: parent.width - width
        y: parent.height / 2 - height / 2
        radius: 13
        color: control.checked ? "#17a81a" : "#ffffff"
        border.color: control.checked ? "#17a81a" : "#cccccc"

        Rectangle {
            x: control.checked ? parent.width - width : 0
            width: 20
            height: 20
            radius: 10
            color: control.down ? "#cccccc" : "#ffffff"
            border.color: control.checked ? (control.down ? "#17a81a" : "#21be2b") : "#999999"
        }
    }

    contentItem: Text {
        text: control.text
        font: control.font
        opacity: enabled ? 1.0 : 0.3
        color: control.checked ? "#21be2b" : "grey"
        verticalAlignment: Text.AlignVCenter
    }
}
