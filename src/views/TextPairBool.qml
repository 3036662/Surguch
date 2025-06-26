import QtQuick
import QtQuick.Controls
import StyleSheet

Column {
    id: root

    property string keyText
    property bool value

    Text {
        id: mainText
        text: keyText
        wrapMode: Text.WordWrap
        maximumLineCount: 5
        width: 200
        font.family: "Noto Sans"
        color: StyleSheet.font_color_extra
    }
    Text {
        text: value ? qsTr("Valid") : qsTr("Invalid")
        color: value ? "green" : "red"
        font.family: "Noto Sans"
    }
}
