import QtQuick
import QtQuick.Controls
import StyleSheet

Switch {
    id: control

    property int size_w: 35
    property int size_h: 20

    font.family: "Noto Sans"

    width: parent.width

    indicator: Rectangle {
        implicitWidth: size_w
        implicitHeight: size_h
        x: parent.width - width
        y: parent.height / 2 - height / 2
        radius: width / 2
        color: control.checked ? "#17a81a" : "#ffffff"
        border.color: control.checked ? "#17a81a" : "#cccccc"

        Rectangle {
            x: control.checked ? parent.width - width : 0
            width: size_h
            height: size_h
            radius: width / 2
            color: control.down ? "#ffffff" : StyleSheet.slider_handle_color
            border.width: control.visualFocus ? 4 : 2
            border.color: control.checked ? (control.down ? "#17a81a" : StyleSheet.slider_border_color) : StyleSheet.slider_border_color
        }
    }
}
