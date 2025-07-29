import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import StyleSheet

ColumnLayout {
    spacing: 1

    Rectangle {
        height: 1
        color: "grey"
        Layout.fillWidth: true
    }

    RowLayout {
        id: toolbar_subpanel
        spacing: 0
        Layout.fillWidth: true

        Text {
            text: "Name"
            Layout.preferredWidth: parent.width * 0.4 - 1
            Layout.maximumWidth: parent.width * 0.4 - 1
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
            text: "Status"
            horizontalAlignment: Text.AlignHCenter
            Layout.preferredWidth: parent.width * 0.1 - 1
            Layout.maximumWidth: parent.width * 0.1 - 1
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
            Layout.maximumWidth: 400
            Layout.preferredWidth: parent.width * 0.25 - 1
            // Layout.maximumWidth: parent.width * 0.25 - 1
            Layout.minimumWidth: 200
            Layout.fillWidth: true
        }
    }
}
