import QtQuick
import QtQuick.Controls
import StyleSheet

Switch {
    id: control
    font.family: "Noto Sans"

    width: parent.width

    indicator: Rectangle {
        implicitWidth: 49
        implicitHeight: 28
        x: parent.width - width
        y: parent.height / 2 - height / 2
        radius: width / 2
        color: control.checked ? "#17a81a" : "#ffffff"
        border.color: control.checked ? "#17a81a" : "#cccccc"

        Rectangle {
            x: control.checked ? parent.width - width : 0
            width: 28
            height: 28
            radius: width / 2
            color: control.down ? "#cccccc" : StyleSheet.slider_handle_color
            border.width: control.visualFocus ? 4 : 2
            border.color: control.checked ? (control.down ? "#17a81a" : StyleSheet.slider_border_color) : StyleSheet.slider_border_color
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
