import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import StyleSheet

ColumnLayout {

    Rectangle {
        height: 1
        color: "grey"
        Layout.fillWidth: true
    }

    RowLayout {
        id: toolbar_subpanel
        spacing: 0
        Layout.fillWidth: true
        Layout.leftMargin: 5

        Text {
            text: "Name"
            Layout.fillWidth: true
            color: StyleSheet.font_color_extra
        }

        HeaderToolSeparator {
        }

        Text {
            text: "Size"
            horizontalAlignment: Text.AlignHCenter
            Layout.preferredWidth: parent.width * 0.1 - 1
            Layout.maximumWidth: parent.width * 0.1 - 1
            color: StyleSheet.font_color_extra
        }

        HeaderToolSeparator {
        }

        Text {
            text: "Last edit"
            horizontalAlignment: Text.AlignHCenter
            Layout.preferredWidth: parent.width * 0.1 - 1
            Layout.maximumWidth: parent.width * 0.1 - 1
            color: StyleSheet.font_color_extra
        }

        HeaderToolSeparator {
        }

        Text {
            text: "Sign"
            horizontalAlignment: Text.AlignHCenter
            Layout.preferredWidth: parent.width * 0.05 - 1
            Layout.maximumWidth: parent.width * 0.05 - 1
            color: StyleSheet.font_color_extra
        }

        HeaderToolSeparator {
        }

        Text {
            text: "MRPA"
            horizontalAlignment: Text.AlignHCenter
            Layout.preferredWidth: parent.width * 0.05 - 1
            Layout.maximumWidth: parent.width * 0.05 - 1
            color: StyleSheet.font_color_extra
        }

        HeaderToolSeparator {
        }

        ToolButton {
            id: deleteAllBtn

            icon.source: StyleSheet.trash_icon
            icon.width: 20
            icon.height: 20
            Layout.preferredWidth: parent.width * 0.05 - 1
            Layout.maximumWidth: parent.width * 0.05 - 1
        }

        HeaderToolSeparator {
        }

        Text {
            text: "Details"
            color: StyleSheet.font_color_extra
            horizontalAlignment: Text.AlignHCenter
            Layout.maximumWidth: 300
            Layout.preferredWidth: 300
            Layout.minimumWidth: 300
            Layout.fillWidth: true
        }
    }
}
