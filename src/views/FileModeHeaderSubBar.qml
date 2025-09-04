import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import StyleSheet

import "header_bar_components" as HeaderBarComponents

ColumnLayout {

    property int sizeColumn: parent.width * 0.1 - 1
    property int editColumn: parent.width * 0.1 - 1
    property int signColumn: parent.width * 0.05 - 1
    property int mrpaColumn: parent.width * 0.05 - 1
    property int deleteColumn: parent.width * 0.05 - 1

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

        HeaderBarComponents.HeaderToolSeparator {
        }

        Text {
            text: "Size"
            horizontalAlignment: Text.AlignHCenter
            Layout.preferredWidth: sizeColumn
            Layout.maximumWidth: sizeColumn
            color: StyleSheet.font_color_extra
        }

        HeaderBarComponents.HeaderToolSeparator {
        }

        Text {
            text: "Last edit"
            horizontalAlignment: Text.AlignHCenter
            Layout.preferredWidth: editColumn
            Layout.maximumWidth: editColumn
            color: StyleSheet.font_color_extra
        }

        HeaderBarComponents.HeaderToolSeparator {
        }

        Text {
            text: "Sign"
            horizontalAlignment: Text.AlignHCenter
            Layout.preferredWidth: parent.width * 0.05 - 1
            Layout.maximumWidth: parent.width * 0.05 - 1
            color: StyleSheet.font_color_extra
        }

        HeaderBarComponents.HeaderToolSeparator {
        }

        Text {
            text: "MRPA"
            horizontalAlignment: Text.AlignHCenter
            Layout.preferredWidth: parent.width * 0.05 - 1
            Layout.maximumWidth: parent.width * 0.05 - 1
            color: StyleSheet.font_color_extra
        }

        HeaderBarComponents.HeaderToolSeparator {
        }

        ToolButton {
            id: deleteAllBtn

            icon.source: StyleSheet.trash_icon
            icon.width: 20
            icon.height: 20
            Layout.preferredWidth: deleteColumn
            Layout.maximumWidth: deleteColumn

            onClicked: {
                fileTreeModel.deleteTree()
            }
        }

        HeaderBarComponents.HeaderToolSeparator {
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
