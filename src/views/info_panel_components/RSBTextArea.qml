import QtQuick
import QtQuick.Controls
import StyleSheet

TextArea {
    id: root
    background: Rectangle {
        border.color: StyleSheet.slider_border_color
        color: StyleSheet.text_area_background
        radius: 4
    }
    width: parent.width
    selectByMouse: true
    wrapMode: Text.WordWrap
    placeholderText: qsTr("Placeholder text")
    placeholderTextColor: StyleSheet.font_color
    color: StyleSheet.font_color
    font.family: "Noto Sans"
}
