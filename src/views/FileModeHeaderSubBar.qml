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
        spacing: 5
        Layout.fillWidth: true

        Text {
            text: "name"
            Layout.preferredWidth: parent.width * 0.4
            color: StyleSheet.font_color_extra
        }

        HeaderToolSeparator {
        }

        Text {
            text: "size"
            Layout.preferredWidth: parent.width * 0.1
            color: StyleSheet.font_color_extra
        }

        HeaderToolSeparator {
        }

        Text {
            text: "Last edit"
            Layout.preferredWidth: parent.width * 0.1
            color: StyleSheet.font_color_extra
        }

        HeaderToolSeparator {
        }

        Text {
            text: "status"
            Layout.preferredWidth: parent.width * 0.1
            color: StyleSheet.font_color_extra
        }

        HeaderToolSeparator {
        }

        ToolButton {
            icon.source: StyleSheet.trash_icon
            icon.width: 20
            icon.height: 20
            Layout.preferredWidth: parent.width * 0.05
        }

        HeaderToolSeparator {
        }

        Text {
            text: "Details"
            color: StyleSheet.font_color_extra
            Layout.maximumWidth: 400
            Layout.preferredWidth: 300
            Layout.minimumWidth: 200
            Layout.fillHeight: true
            Layout.fillWidth: true
        }
    }
}
