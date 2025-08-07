import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Controls.impl
import alt.pdfcsp.fileTreeModel
import StyleSheet

TreeView {
    id: treeView

    Layout.fillHeight: true
    Layout.leftMargin: 5
    Layout.alignment: Qt.AlignLeft

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

        function getImageForNode(type) {
            switch (type) {
                case "Zip" :
                    return StyleSheet.box_icon;
                case "Dir":
                    return StyleSheet.folder_plus_icon;
                case "File":
                    return StyleSheet.file_simple_icon;
                case "Sig":
                    return StyleSheet.medal_icon;
                case "Asig":
                    return StyleSheet.box_icon;
                case "Mrpa":
                    return StyleSheet.file_simple_icon;
                default:
                    return "";
            }
        }

        implicitHeight: nameField.implicitHeight * 1.6
        implicitWidth: treeView.width - padding * 2

        Rectangle {
            id: background

            anchors.fill: parent
            color: "transparent"
            border.color: row === treeView.currentRow ? "#3daee9" : "transparent"
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
                        let index = treeView.index(row, column);
                        treeView.selectionModel.setCurrentIndex(index, ItemSelectionModel.NoUpdate);
                        treeView.toggleExpanded(row);
                    }
                }
            }
            Rectangle {
                id: treeIndent

                color: "transparent"

                Layout.fillHeight: true
                Layout.minimumWidth: hasChildren ? indentation : indentation + 10
                Layout.maximumWidth: hasChildren ? indentation : indentation + 10
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
                visible: model.type === "temp"
                Layout.preferredWidth: sizeColumn
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
                visible: model.type === "temp"
                Layout.preferredWidth: editColumn
            }

            Label {
                id: signStatusField

                Layout.alignment: Qt.AlignLeft
                Layout.preferredWidth: signColumn
                clip: true
                color: StyleSheet.font_color_extra
                font.pixelSize: column === 0 ? 14 : 10
                horizontalAlignment: Text.AlignHCenter
                text: model.type
                visible: model.sig_status

                background: Rectangle {
                    color: "green"
                    width: signStatusField.contentWidth + 12
                    height: signStatusField.contentHeight + 6
                    radius: height / 4
                    anchors.centerIn: parent
                    visible: signStatusField.text !== ""

                    MouseArea {
                        id: sigStatusArea

                        anchors.fill: parent
                        hoverEnabled: true

                        onClicked: {
                            console.warn("before click " + rightSideBar.visible)
                            rightSideBar.showState = RightSideBar.ShowState.SigInfo;
                            console.warn("after click " + rightSideBar.visible)
                        }
                        onEntered: {
                            cursorShape = Qt.PointingHandCursor;
                        }
                        onExited: {
                            cursorShape = Qt.ArrowCursor;
                        }
                    }
                }
            }

            BusyIndicator {
                id: sign_busy_indicator
                visible: !model.sig_status
                Layout.preferredWidth: signColumn
            }

            Label {
                id: mrpaStatusField

                Layout.alignment: Qt.AlignLeft
                Layout.preferredWidth: mrpaColumn
                clip: true
                color: StyleSheet.font_color_extra
                font.pixelSize: column === 0 ? 14 : 10
                horizontalAlignment: Text.AlignHCenter
                text: model.sig_status
                visible: model.sig_status

                background: Rectangle {
                    color: "green"
                    width: mrpaStatusField.contentWidth + 12
                    height: mrpaStatusField.contentHeight + 6
                    radius: height / 4
                    anchors.centerIn: parent
                    visible: mrpaStatusField.text !== ""

                    MouseArea {
                        id: mrpaStatusArea

                        anchors.fill: parent
                        hoverEnabled: true

                        onClicked: {
                            rightSideBar.showState = RightSideBar.ShowState.Certs;
                        }
                        onEntered: {
                            cursorShape = Qt.PointingHandCursor;
                        }
                        onExited: {
                            cursorShape = Qt.ArrowCursor;
                        }
                    }
                }
            }

            BusyIndicator {
                id: mrpa_busy_indicator

                visible: !model.sig_status
                Layout.preferredWidth: mrpaColumn
            }

            ToolButton {
                id: deleteBtn

                visible: depth === 0
                Layout.preferredWidth: deleteColumn
                icon.height: 20
                icon.source: StyleSheet.trash_icon
                icon.width: 20
                height: 20
                width: 20

                onClicked: {
                    fileTreeModel.deleteNode(row, model.uid)
                }
            }

            Item {
                id: dummyDelete

                visible: !deleteBtn.visible
                Layout.preferredWidth: deleteColumn
            }
        }

    }
    selectionModel: ItemSelectionModel {
        model: treeView.model
    }
}