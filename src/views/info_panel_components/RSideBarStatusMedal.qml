import QtQuick
import QtQuick.Layouts
import StyleSheet


RowLayout {
    width: parent.width
    height: 50
    Layout.preferredHeight: 30

    property string title
    property bool value: false

    TextPairBool {
        Layout.alignment: Qt.AlignLeft
        keyText: title
        value: parent.value
    }

    Image {
        Layout.maximumHeight: 20
        Layout.maximumWidth: 20
        Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
        Layout.rightMargin: 40
        Layout.topMargin: 5
        source: value ? StyleSheet.medal_green_icon : StyleSheet.medal_pink_icon
    }
}
