import QtQuick
import QtQuick.Controls
import StyleSheet

Column {
    id: root

    property string keyText
    property bool value
    property alias status_text_color: status_text.color

    Text {
        id: mainText
        text: keyText
        wrapMode: Text.WordWrap
        maximumLineCount: 5
        width: 200
        font.family: "Noto Sans"
        font.pixelSize: 12
        color: StyleSheet.font_color_extra
    }
    Text {
        id: status_text
        text: value ? qsTr("Valid") : qsTr("Invalid")
        color: value ? "green" : "red"
        font.family: "Noto Sans"
        font.pixelSize: 12
    }
}
