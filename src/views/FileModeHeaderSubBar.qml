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

    Item {
        id: toolbar_subpanel
        Layout.fillWidth: true
        Layout.preferredHeight: 30

        Text {
            id: nameText
            text: qsTr("Name")
            clip: true
            anchors.left: parent.left
            anchors.right: sepName.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            leftPadding: 15
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignLeft
            elide: Text.ElideRight
            color: StyleSheet.font_color_extra
            font.pixelSize: 12
        }
        HeaderBarComponents.HeaderToolSeparator {
            id: sepName
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.right: sizeText.left
            width: 1
            leftPadding: 0
            rightPadding: 0
            topPadding: 0
            bottomPadding: 0
        }

        Text {
            id: sizeText
            text: qsTr("Size")
            clip: true
            width: sizeColumn
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: sepSize.left
            color: StyleSheet.font_color_extra
            font.pixelSize: 12
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }
        HeaderBarComponents.HeaderToolSeparator {
            id: sepSize
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.right: editText.left
            width: 1
            leftPadding: 0
            rightPadding: 0
            topPadding: 0
            bottomPadding: 0
        }

        Text {
            id: editText
            text: qsTr("Last edit")
            clip: true
            width: editColumn
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: sepEdit.left
            color: StyleSheet.font_color_extra
            font.pixelSize: 12
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }
        HeaderBarComponents.HeaderToolSeparator {
            id: sepEdit
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.right: signText.left
            width: 1
            leftPadding: 0
            rightPadding: 0
            topPadding: 0
            bottomPadding: 0
        }

        Text {
            id: signText
            text: qsTr("Sign")
            clip: true
            width: signColumn
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: sepSign.left
            color: StyleSheet.font_color_extra
            font.pixelSize: 12
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }
        HeaderBarComponents.HeaderToolSeparator {
            id: sepSign
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.right: mrpaText.left
            width: 1
            leftPadding: 0
            rightPadding: 0
            topPadding: 0
            bottomPadding: 0
        }

        Text {
            id: mrpaText
            text: qsTr("MRPA")
            clip: true
            width: mrpaColumn
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: sepDeleteBtn.left
            color: StyleSheet.font_color_extra
            font.pixelSize: 12
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }
        HeaderBarComponents.HeaderToolSeparator {
            id: sepDeleteBtn
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.right: deleteAllBtn.left
            width: 1
            leftPadding: 0
            rightPadding: 0
            topPadding: 0
            bottomPadding: 0
        }

        ToolButton {
            id: deleteAllBtn
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: sepDetails.left
            width: deleteColumn
            icon.source: StyleSheet.trash_icon
            icon.width: 20
            icon.height: 20
            onClicked: {
                fileTreeModel.deleteTree()
                rightSideBar.showState = RightSideBar.ShowState.Invisible
            }
        }
        HeaderBarComponents.HeaderToolSeparator {
            id: sepDetails
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.right: detailText.left
            width: 1
            leftPadding: 0
            rightPadding: 0
            topPadding: 0
            bottomPadding: 0
        }

        Text {
            id: detailText
            text: qsTr("Details")
            clip: true
            width: 300
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            color: StyleSheet.font_color_extra
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
        }
    }
}
