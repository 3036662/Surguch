import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import StyleSheet

RowLayout {
    id: root

    property alias labelText: lbl.text
    property alias checked: switch1.checked

    signal toggled();

    Text {
        id:lbl

        Layout.fillWidth: true

        color: StyleSheet.font_color_extra
        font.family: StyleSheet.defaultFontFamily
        font.pointSize: StyleSheet.defaultTextPointSize
        topPadding: 10
        bottomPadding: 10
    }

    Rectangle {
        Layout.fillWidth: true

    }

    SettingSwitch {
        id: switch1
        topPadding: 10
        bottomPadding: 10
        rightPadding: 10
        rightInset: 10

        onToggled: {
            root.toggled()
        }
    }
}
