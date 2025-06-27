import QtQuick
import QtQuick.Controls
import StyleSheet

Column {
    id: root

    property string keyText
    property string value
    property int textWidth

    width: parent.width

    Text {
        text: keyText
        font.family: "Noto Sans"
        color: StyleSheet.font_color_extra
    }
    TextArea {
        background: Rectangle {
            border.color: "transparent"
            color: "transparent"
        }

        width: parent.width
        readOnly: true
        selectByMouse: true
        wrapMode: Text.WordWrap
        //maximumLineCount: 5
        text: value
        //color: "grey"
        font.family: "Noto Sans"
        color: StyleSheet.font_color_extra
    }
}
