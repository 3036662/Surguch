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
        implicitWidth: column === 0 ? treeView.width * 2 / 3 - padding * 2 : treeView.width * 1 / 3 - padding * 2
        implicitHeight: label.implicitHeight * 1.5

        readonly property real indentation: 20
        readonly property real padding: 5

        // Assigned to by TreeView:
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
            color: row === treeView.currentRow ? "#0085F8" : "transparent"
        }


        RowLayout {
            Layout.fillWidth: true

            Rectangle {
                Layout.preferredWidth: indentation * depth
                color: "transparent"
            }

            Image {
                id: indicator
                visible: isTreeNode && hasChildren
                //text: expanded ? "▼" : "▶"
                //color: "white"
                source: expanded ? StyleSheet.chevron_down : StyleSheet.chevron_right
                width: 20
                height: 20

                TapHandler {
                    onSingleTapped: {
                        let index = treeView.index(row, column)
                        treeView.selectionModel.setCurrentIndex(index, ItemSelectionModel.NoUpdate)
                        treeView.toggleExpanded(row)
                        console.warn(depth)
                    }
                }
            }

            Rectangle {
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
                id: label
                //x: indicator.x + indicator.width + 5
                //anchors.verticalCenter: parent.verticalCenter
                //width: parent.width - padding - x
                clip: true
                text: model.file_name
                color: column === 0 ? "white" : "#ABABAB"
                font.pixelSize: column === 0 ? 14 : 10
                //horizontalAlignment: isTreeNode ? Text.AlignLeft : Text.AlignRight
                rightPadding: 10
            }

            Label {
                id: label2
                //x: indicator.x + indicator.width + 5
                //anchors.verticalCenter: parent.verticalCenter
                //width: parent.width - padding - x
                clip: true
                text: model.size
                color: column === 0 ? "white" : "#ABABAB"
                font.pixelSize: column === 0 ? 14 : 10
                //horizontalAlignment: isTreeNode ? Text.AlignLeft : Text.AlignRight
                rightPadding: 10
            }

            Label {
                id: label3
                //x: indicator.x + indicator.width + 5
                //anchors.verticalCenter: parent.verticalCenter
                //width: parent.width - padding - x
                clip: true
                text: model.last_edit
                color: column === 0 ? "white" : "#ABABAB"
                font.pixelSize: column === 0 ? 14 : 10
                //horizontalAlignment: isTreeNode ? Text.AlignLeft : Text.AlignRight
                rightPadding: 10
            }

            Label {
                id: label4
                //x: indicator.x + indicator.width + 5
                //anchors.verticalCenter: parent.verticalCenter
                //width: parent.width - padding - x
                clip: true
                text: model.status
                color: column === 0 ? "white" : "#ABABAB"
                font.pixelSize: column === 0 ? 14 : 10
                //horizontalAlignment: isTreeNode ? Text.AlignLeft : Text.AlignRight
                rightPadding: 10
            }
        }
    }
}