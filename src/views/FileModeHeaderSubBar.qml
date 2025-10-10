import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import StyleSheet

import "header_bar_components" as HeaderBarComponents

ColumnLayout {

    property int sizeColumn: parent.width * 0.05
    property int editColumn: parent.width * 0.1
    property int signColumn: parent.width * 0.05
    property int mrpaColumn: parent.width * 0.05
    property int deleteColumn: parent.width * 0.05

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
            text: qsTr("Name")
            Layout.fillWidth: true
            color: StyleSheet.font_color_extra
            font.pixelSize: 12
        }

        HeaderBarComponents.HeaderToolSeparator {
            padding: 0
        }

        Text {
            text: qsTr("Size")
            horizontalAlignment: Text.AlignHCenter
            Layout.preferredWidth: sizeColumn
            Layout.minimumWidth: sizeColumn
            Layout.maximumWidth: sizeColumn
            color: StyleSheet.font_color_extra
            font.pixelSize: 12
        }

        HeaderBarComponents.HeaderToolSeparator {
            padding: 0
        }

        Text {
            text: qsTr("Last edit")
            horizontalAlignment: Text.AlignHCenter
            Layout.preferredWidth: editColumn
            Layout.minimumWidth: editColumn
            Layout.maximumWidth: editColumn
            color: StyleSheet.font_color_extra
            font.pixelSize: 12
        }

        HeaderBarComponents.HeaderToolSeparator {
            padding: 0
        }

        Text {
            text: qsTr("Sign")
            horizontalAlignment: Text.AlignHCenter
            Layout.preferredWidth: signColumn
            Layout.minimumWidth: signColumn
            Layout.maximumWidth: signColumn
            color: StyleSheet.font_color_extra
            font.pixelSize: 12
        }

        HeaderBarComponents.HeaderToolSeparator {
            padding: 0
        }

        Text {
            text: qsTr("MRPA")
            horizontalAlignment: Text.AlignHCenter
            Layout.preferredWidth: mrpaColumn
            Layout.minimumWidth: mrpaColumn
            Layout.maximumWidth: mrpaColumn
            color: StyleSheet.font_color_extra
            font.pixelSize: 12
        }

        HeaderBarComponents.HeaderToolSeparator {
            padding: 0
        }

        ToolButton {
            id: deleteAllBtn

            icon.source: StyleSheet.trash_icon
            icon.width: 20
            icon.height: 20
            Layout.preferredWidth: deleteColumn
            Layout.minimumWidth: deleteColumn
            Layout.maximumWidth: deleteColumn

            onClicked: {
                fileTreeModel.deleteTree()
                rightSideBar.showState = RightSideBar.ShowState.Invisible
            }
        }

        HeaderBarComponents.HeaderToolSeparator {
            padding: 0
        }

        Text {
            text: qsTr("Details")
            color: StyleSheet.font_color_extra
            horizontalAlignment: Text.AlignHCenter
            Layout.maximumWidth: 300
            Layout.preferredWidth: 300
            Layout.minimumWidth: 300
        }
    }
}
