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
    Layout.minimumWidth: 200
    Layout.alignment: Qt.AlignHCenter

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
                        //console.warn(nameField.width);
                    }
                }
            }
            Rectangle {
                id: treeIndent

                Layout.preferredWidth: hasChildren ? indentation : indentation + 20
                Layout.minimumWidth: hasChildren ? indentation : indentation + 20
                Layout.maximumWidth: hasChildren ? indentation : indentation + 20
            }
            Image {
                id: image

                height: 15
                source: getImageForNode(model.type)
                visible: column === 0 ? true : false
                width: 15
            }
            Label {
                id: nameField

                Layout.alignment: Qt.AlignLeft
                Layout.maximumWidth: treeView.width * 0.4 - indent.width - treeIndent.width - image.width - indicator.width
                Layout.minimumWidth: treeView.width * 0.4 - indent.width - treeIndent.width - image.width - indicator.width
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
                Layout.preferredWidth: treeView.width * 0.1
                clip: true
                color: StyleSheet.font_color_extra
                font.pixelSize: column === 0 ? 14 : 10
                text: model.size
                background: Rectangle {
                    anchors.fill: parent
                    color: "transparent"
                    border.color: "red"
                }
            }
            Label {
                id: lastEditField

                Layout.alignment: Qt.AlignLeft
                Layout.preferredWidth: treeView.width * 0.1
                clip: true
                color: StyleSheet.font_color_extra
                font.pixelSize: column === 0 ? 14 : 10
                text: model.modification_time
                background: Rectangle {
                    anchors.fill: parent
                    color: "transparent"
                    border.color: "red"
                }
            }
            Label {
                id: signStatusField

                Layout.alignment: Qt.AlignLeft
                Layout.preferredWidth: treeView.width * 0.05
                clip: true
                color: StyleSheet.font_color_extra
                font.pixelSize: column === 0 ? 14 : 10
                horizontalAlignment: Text.AlignHCenter
                text: model.sig_status

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

            Label {
                id: mrpaStatusField

                Layout.alignment: Qt.AlignLeft
                Layout.preferredWidth: treeView.width * 0.05
                clip: true
                color: StyleSheet.font_color_extra
                font.pixelSize: column === 0 ? 14 : 10
                horizontalAlignment: Text.AlignHCenter
                text: model.sig_status

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
                            rightSideBar.showState = RightSideBar.ShowState.SigInfo;
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

            ToolButton {
                id: deleteBtn

                visible: depth === 0
                Layout.preferredWidth: treeView.width * 0.05
                icon.height: 20
                icon.source: StyleSheet.trash_icon
                icon.width: 20
                height: 20
                width: 20
            }

            Item {
                id: dummyDelete

                visible: !deleteBtn.visible
                Layout.preferredWidth: treeView.width * 0.05
            }

            Rectangle {
                id: dummyItem

                visible: !rightSideBar.visible
                color: "transparent"
                border.color: "red"
                Layout.fillWidth: true
                Layout.maximumWidth: 300
                Layout.minimumWidth: 300
                Layout.preferredWidth: 300
            }
        }

    }
    selectionModel: ItemSelectionModel {
        model: treeView.model
    }
}