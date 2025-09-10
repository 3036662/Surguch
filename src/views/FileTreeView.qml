import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.impl
import alt.pdfcsp.fileTreeModel
import StyleSheet

TreeView {
    id: treeView

    Layout.alignment: Qt.AlignLeft
    Layout.fillHeight: true
    Layout.leftMargin: 5
    clip: true
    model: fileTreeModel

    delegate: Item {
        required property int column
        required property bool current
        required property int depth
        required property bool expanded
        required property int hasChildren
        readonly property real indentation: 30
        required property bool isTreeNode
        readonly property real padding: 5
        required property int row
        required property TreeView treeView

        //switch according to enum Types in tree_item.hpp
        function getImageForNode(type) {
            switch (type) {
            case 0:
                return StyleSheet.box_icon
            case 1:
                return StyleSheet.folder_plus_icon
            case 2:
                return StyleSheet.file_simple_icon
            case 3:
                return StyleSheet.medal_icon
            case 4:
                return StyleSheet.box_icon
            case 5:
                return StyleSheet.file_simple_icon
            default:
                return ""
            }
        }

        implicitHeight: nameField.implicitHeight * 1.6
        implicitWidth: treeView.width - padding * 2

        Rectangle {
            id: background

            anchors.fill: parent
            border.color: row === treeView.currentRow ? "#3daee9" : "transparent"
            color: "transparent"
        }
        RowLayout {
            anchors.fill: parent
            spacing: 0

            Rectangle {
                id: indent

                Layout.preferredWidth: indentation * depth
                color: "transparent"
            }
            Image {
                id: indicator

                height: 20
                source: expanded ? StyleSheet.chevron_down : StyleSheet.chevron_right
                sourceSize.height: expanded ? 10 : 15
                sourceSize.width: expanded ? 20 : 10
                visible: isTreeNode && hasChildren
                width: 20

                TapHandler {
                    onSingleTapped: {
                        let index = treeView.index(row, column)
                        treeView.selectionModel.setCurrentIndex(
                                    index, ItemSelectionModel.NoUpdate)
                        treeView.toggleExpanded(row)
                    }
                }
            }
            Rectangle {
                id: treeIndent

                Layout.fillHeight: true
                Layout.maximumWidth: hasChildren ? indentation : indentation + 10
                Layout.minimumWidth: hasChildren ? indentation : indentation + 10
                color: "transparent"
            }
            Image {
                id: image

                height: 15
                source: getImageForNode(model.type)
                visible: column === 0 && model.type !== "temp"
                width: 15
            }
            Label {
                id: nameField

                Layout.fillWidth: true
                Layout.leftMargin: 5
                ToolTip.delay: 500
                ToolTip.text: model.name
                ToolTip.visible: nameArea.containsMouse
                clip: true
                color: StyleSheet.font_color_extra
                font.pixelSize: column === 0 ? 16 : 10
                text: model.name

                MouseArea {
                    id: nameArea

                    acceptedButtons: Qt.NoButton
                    anchors.fill: parent
                    hoverEnabled: true
                }
            }
            Label {
                id: sizeField

                Layout.alignment: Qt.AlignLeft
                Layout.preferredWidth: sizeColumn
                clip: true
                color: StyleSheet.font_color_extra
                font.pixelSize: column === 0 ? 14 : 10
                horizontalAlignment: Text.AlignHCenter
                text: model.size
                visible: model.type !== "temp"
            }
            BusyIndicator {
                id: size_busy_indicator

                Layout.preferredWidth: sizeColumn
                visible: model.type === "temp"
            }
            Label {
                id: lastEditField

                Layout.alignment: Qt.AlignLeft
                Layout.preferredWidth: editColumn
                clip: true
                color: StyleSheet.font_color_extra
                font.pixelSize: column === 0 ? 14 : 10
                horizontalAlignment: Text.AlignHCenter
                text: model.modification_time
                visible: model.type !== "temp"
            }
            BusyIndicator {
                id: date_busy_indicator

                Layout.preferredWidth: editColumn
                visible: model.type === "temp"
            }
            Label {
                id: signStatusField

                Layout.alignment: Qt.AlignLeft
                Layout.preferredWidth: signColumn
                clip: true
                color: StyleSheet.font_color_extra
                font.pixelSize: column === 0 ? 14 : 10
                horizontalAlignment: Text.AlignHCenter
                text: model.sig_status
                visible: signStatusField.text !== "" && !fileTreeModel.isDraft

                background: Rectangle {
                    anchors.centerIn: parent
                    color: model.sig_color
                    height: signStatusField.contentHeight + 6
                    radius: height / 4
                    visible: signStatusField.text !== ""
                             && !fileTreeModel.isDraft
                    width: signStatusField.contentWidth + 12

                    MouseArea {
                        id: sigStatusArea

                        anchors.fill: parent
                        hoverEnabled: true

                        onClicked: {
                            rightSideBar.showState = RightSideBar.ShowState.Certs
                        }
                        onEntered: {
                            cursorShape = Qt.PointingHandCursor
                        }
                        onExited: {
                            cursorShape = Qt.ArrowCursor
                        }
                    }
                }
            }
            Rectangle {
                id: sign_status_dummy

                Layout.alignment: Qt.AlignLeft
                Layout.preferredWidth: signColumn
                visible: !signStatusField.visible
                         && !sign_busy_indicator.visible
            }
            BusyIndicator {
                id: sign_busy_indicator

                Layout.preferredWidth: signColumn
                visible: fileTreeModel.isDraft
            }
            Label {
                id: mrpaStatusField

                Layout.alignment: Qt.AlignLeft
                Layout.preferredWidth: mrpaColumn
                clip: true
                color: StyleSheet.font_color_extra
                font.pixelSize: column === 0 ? 14 : 10
                horizontalAlignment: Text.AlignHCenter
                text: model.mrpa_status
                visible: !fileTreeModel.isDraft

                background: Rectangle {
                    anchors.centerIn: parent
                    color: model.mrpa_color
                    height: mrpaStatusField.contentHeight + 6
                    radius: height / 4
                    visible: mrpaStatusField.text !== ""
                             && !fileTreeModel.isDraft
                    width: mrpaStatusField.contentWidth + 12

                    MouseArea {
                        id: mrpaStatusArea

                        anchors.fill: parent
                        hoverEnabled: true

                        onClicked: {
                            rightSideBar.showState = RightSideBar.ShowState.Certs
                        }
                        onEntered: {
                            cursorShape = Qt.PointingHandCursor
                        }
                        onExited: {
                            cursorShape = Qt.ArrowCursor
                        }
                    }
                }
            }
            Rectangle {
                id: mrpa_status_dummy

                Layout.alignment: Qt.AlignLeft
                Layout.preferredWidth: signColumn
                visible: !mrpaStatusField.visible
                         && !mrpa_busy_indicator.visible
            }
            BusyIndicator {
                id: mrpa_busy_indicator

                Layout.preferredWidth: mrpaColumn
                visible: fileTreeModel.isDraft
            }
            ToolButton {
                id: deleteBtn

                Layout.preferredWidth: deleteColumn
                height: 20
                icon.height: 20
                icon.source: StyleSheet.trash_icon
                icon.width: 20
                visible: depth === 0
                width: 20

                onClicked: {
                    console.warn("delete node res: " + fileTreeModel.deleteNode(
                                     model.full_path, row, model.uid, model.id))
                }
            }
            Item {
                id: dummyDelete

                Layout.preferredWidth: deleteColumn
                visible: !deleteBtn.visible
            }
        }
    }
    selectionModel: ItemSelectionModel {
        model: treeView.model
    }
}
