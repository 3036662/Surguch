import QtQuick
import QtQuick.Controls
import StyleSheet

Column {
    id: root

    property string keyText
    property string value
    property int textWidth
    property bool isMrpa: false

    width: parent.width

    Text {
        text: keyText
        width: parent.width
        font.family: "Noto Sans"
        font.pixelSize: 12
        wrapMode: isMrpa ? TextEdit.WrapAnywhere : Text.WordWrap
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
        wrapMode: isMrpa ? TextEdit.WrapAnywhere : Text.WordWrap
        //maximumLineCount: 5
        text: value
        //color: "grey"
        font.family: "Noto Sans"
        font.pixelSize: 12
        color: StyleSheet.font_color_extra

        MouseArea {
            anchors.fill: parent
            cursorShape: value.startsWith(
                             "http") ? Qt.PointingHandCursor : Qt.ArrowCursor
            hoverEnabled: value.startsWith("http")
            enabled: value.startsWith("http")
            onClicked: {
                if (value) {
                    Qt.openUrlExternally(value)
                }
            }
        }
    }
}
