import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.impl
import alt.pdfcsp.fileTreeModel
import StyleSheet

TreeView {
    id: treeView

    Layout.fillHeight: true
    Layout.fillWidth: true
    Layout.leftMargin: 5
    Layout.rightMargin: 5
    //Layout.minimumWidth: 200
    Layout.alignment: Qt.AlignHCenter
    clip: false
    model: fileTreeModel
    selectionModel: ItemSelectionModel {
        model: treeView.model
    }

    delegate: Item {
        implicitWidth: treeView.width - padding * 2
        implicitHeight: nameField.implicitHeight * 1.6

        readonly property real indentation: 20
        readonly property real padding: 5


        required property TreeView treeView
        required property bool isTreeNode
        required property bool expanded
        required property int hasChildren
        required property int depth
        required property int row
        required property int column
        required property bool current

        Rectangle {
            id: background
            anchors.fill: parent
            color: "transparent"
            border.color: row === treeView.currentRow ? "#3daee9" : "transparent"
        }


        RowLayout {
            anchors.fill: parent

            Rectangle {
                id: indent
                Layout.preferredWidth: indentation * depth
                color: "transparent"
            }

            Image {
                id: indicator
                visible: isTreeNode && hasChildren
                source: expanded ? StyleSheet.chevron_down : StyleSheet.chevron_right
                width: 20
                height: 20

                TapHandler {
                    onSingleTapped: {
                        let index = treeView.index(row, column)
                        treeView.selectionModel.setCurrentIndex(index, ItemSelectionModel.NoUpdate)
                        treeView.toggleExpanded(row)
                        console.warn(nameField.width)
                    }
                }
            }

            Rectangle {
                id: treeIndent
                Layout.preferredWidth: isTreeNode && hasChildren ? indentation : indentation + indicator.width
            }

            Image {
                id: image
                width: 13
                height: 13
                source:  hasChildren ? StyleSheet.file_plus_icon : StyleSheet.file_simple_icon
                visible: column === 0 ? true : false
            }

            Label {
                id: nameField
                Layout.alignment: Qt.AlignLeft
                Layout.preferredWidth: parent.width * 0.4 - indent.width - treeIndent.width - image.width - indicator.width
                Layout.maximumWidth: parent.width * 0.4 - indent.width - treeIndent.width - image.width - indicator.width
                clip: true
                text: model.name
                color: column === 0 ? "white" : "#ABABAB"
                font.pixelSize: column === 0 ? 16 : 10
                background: Rectangle {
                    color: "transparent"
                }
            }

            Label {
                id: sizeField
                Layout.alignment: Qt.AlignLeft
                Layout.preferredWidth: parent.width * 0.1
                clip: true
                text: model.size
                color: column === 0 ? "white" : "#ABABAB"
                font.pixelSize: column === 0 ? 14 : 10
            }

            Label {
                id: lastEditField
                Layout.alignment: Qt.AlignLeft
                Layout.preferredWidth: parent.width * 0.1
                clip: true
                text: model.modification_time
                color: column === 0 ? "white" : "#ABABAB"
                font.pixelSize: column === 0 ? 14 : 10
            }

            Label {
                id: statusField
                horizontalAlignment: Text.AlignHCenter
                Layout.alignment: Qt.AlignLeft
                Layout.preferredWidth: parent.width * 0.10
                clip: true
                text: model.status
                color: column === 0 ? "white" : "#ABABAB"
                font.pixelSize: column === 0 ? 14 : 10

                background: Rectangle {
                    id: statusBackground
                    color: "green"
                    width: statusField.contentWidth + 12
                    height: statusField.contentHeight + 6
                    radius: height / 4
                    anchors.centerIn: parent
                    visible: statusField.text !== ""
                }
            }

            ToolButton {
                id: deleteBtn
                icon.source: StyleSheet.trash_icon
                icon.width: 20
                icon.height: 20
                Layout.preferredWidth: parent.width * 0.05
            }

            Item {
                Layout.fillWidth: true
                Layout.maximumWidth: 400
                Layout.preferredWidth: 300
                Layout.minimumWidth: 200
            }
        }
    }
}