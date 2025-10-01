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
    Layout.maximumWidth: parent.width - 300
    Layout.minimumWidth: parent.width - 300
    clip: true
    model: fileTreeModel

    signal showMrpaList(var data)
    signal cleanWindow
    signal enableSignButton

    function gatherParamsTree(path) {
        let curr_profile = {}
        let cert_array = {}
        curr_profile = JSON.parse(header.getCurrentProfileValue())
        cert_array = JSON.parse(profilesModel.getUserCertsJSON())
        let cert_index = cert_array.findIndex(cert => {
                                                  return curr_profile.cert_serial === cert.serial
                                              })
        if (cert_index === -1) {
            errorMessageDialog.text = qsTr(
                        "Certificate not found, looks like it was deleted.﻿")
            errorMessageDialog.open()
            throw new Error('Certificate data not found')
        }

        console.warn(JSON.stringify(curr_profile))
        console.warn("---------------------------------")

        let params = {
            "cert_serial": curr_profile.cert_serial,
            "cert_subject": cert_array[cert_index].subject_common_name,
            "cades_type": curr_profile.CADES_format,
            "tsp_url": curr_profile.tsp_url,
            "sig_ext": curr_profile.sig_ext,
            "dest_dir_path": path,
            "create_attached": curr_profile.create_attached,
            "create_base_64_encoded": curr_profile.create_base_64_encoded,
            "pack_to_zip": curr_profile.pack_to_zip,
            "pack_separate_zips": curr_profile.pack_separate_zips
        }
        console.warn(JSON.stringify(params))
        fileTreeModel.signTree(params)
    }

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

        function convertSizes(bytes) {
            if (bytes < 1024)
                return bytes + qsTr(" B")
            else if (bytes < 1024 * 1024)
                return (bytes / 1024).toFixed(1) + qsTr(" KB")
            else if (bytes < 1024 * 1024 * 1024)
                return (bytes / (1024 * 1024)).toFixed(1) + qsTr(" MB")
        }

        function getSignIcon(color) {
            switch (color) {
            case "sig_green":
                return StyleSheet.medal_green_icon
            case "sig_red":
                return StyleSheet.medal_pink_icon
            case "sig_mixed":
                return StyleSheet.medal_icon
            case "file_green":
                return StyleSheet.cell_icon_green
            case "file_red":
                return StyleSheet.cell_icon_red
            case "file_mixed":
                return StyleSheet.cell_icon
            default:
                return ""
            }
        }

        function getMrpaIcon(color) {
            switch (color) {
            case "mrpa_green":
                return StyleSheet.medal_green_icon
            case "mrpa_red":
                return StyleSheet.medal_pink_icon
            case "mrpa_mixed":
                return StyleSheet.medal_icon
            case "file_green":
                return StyleSheet.cell_icon_green
            case "file_red":
                return StyleSheet.cell_icon_red
            case "file_mixed":
                return StyleSheet.cell_icon
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
        Item {
            anchors.fill: parent

            Item {
                id: indent

                anchors.left: parent.left
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                width: indentation * depth
            }
            Item {
                id: indicatorItem
                anchors.left: indent.right
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                width: 20

                Image {
                    id: indicator

                    anchors.centerIn: parent
                    source: expanded ? StyleSheet.chevron_down : StyleSheet.chevron_right
                    sourceSize.height: expanded ? 10 : 15
                    sourceSize.width: expanded ? 20 : 10
                    visible: isTreeNode && hasChildren

                    TapHandler {
                        onSingleTapped: {
                            let index = treeView.index(row, column)
                            treeView.selectionModel.setCurrentIndex(
                                        index, ItemSelectionModel.NoUpdate)
                            treeView.toggleExpanded(row)
                        }
                    }
                }
            }
            Item {
                id: treeIndent

                anchors.left: indicatorItem.right
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                width: hasChildren ? indentation : indentation + 10
            }
            Item {
                id: imageItem
                anchors.left: treeIndent.right
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.rightMargin: 3
                width: height

                Image {
                    id: image

                    anchors.fill: parent
                    source: getImageForNode(model.type)
                    visible: column === 0 && model.type !== "temp"
                }
            }

            Item {
                id: nameItem

                anchors.left: imageItem.right
                anchors.right: sizeItem.left
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.rightMargin: 3
                clip: true

                Text {
                    id: nameField

                    anchors.fill: parent
                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter
                    maximumLineCount: 1
                    elide: Text.ElideRight
                    wrapMode: Text.NoWrap
                    ToolTip.delay: 500
                    ToolTip.text: model.name
                    ToolTip.visible: nameArea.containsMouse
                    clip: true
                    color: StyleSheet.font_color_extra
                    font.pixelSize: column === 0 ? 14 : 10
                    text: model.name

                    MouseArea {
                        id: nameArea

                        acceptedButtons: Qt.NoButton
                        anchors.fill: parent
                        hoverEnabled: true
                    }
                }
            }

            Item {
                id: sizeItem
                anchors.right: dateItem.left
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                width: sizeColumn
                anchors.rightMargin: 3
                clip: true

                Text {
                    id: sizeField

                    anchors.centerIn: parent
                    width: sizeColumn
                    ToolTip.delay: 500
                    ToolTip.text: convertSizes(model.size)
                    ToolTip.visible: sizeArea.containsMouse
                    clip: true
                    color: StyleSheet.font_color_extra
                    font.pixelSize: column === 0 ? 14 : 10
                    horizontalAlignment: Text.AlignHCenter
                    text: convertSizes(model.size)
                    // model.type = 1 is Dir which in our case doesn't have size or creation Date
                    visible: model.type !== "temp" && model.type !== 1

                    MouseArea {
                        id: sizeArea

                        acceptedButtons: Qt.NoButton
                        anchors.fill: parent
                        hoverEnabled: true
                    }
                }
                Rectangle {
                    id: size_dummy

                    anchors.fill: parent
                    color: "transparent"
                    visible: model.type === 1
                }

                BusyIndicator {
                    id: size_busy_indicator

                    anchors.centerIn: parent
                    width: sizeColumn
                    visible: model.type === "temp"
                }
            }

            Item {
                id: dateItem
                anchors.right: signItem.left
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.rightMargin: 3
                width: editColumn
                clip: true

                Text {
                    id: lastEditField

                    anchors.centerIn: parent
                    width: editColumn
                    ToolTip.delay: 500
                    ToolTip.text: model.modification_time
                    ToolTip.visible: dateArea.containsMouse
                    clip: true
                    color: StyleSheet.font_color_extra
                    font.pixelSize: column === 0 ? 14 : 10
                    horizontalAlignment: Text.AlignHCenter
                    text: model.modification_time
                    visible: model.type !== "temp" && model.type !== 1

                    MouseArea {
                        id: dateArea

                        acceptedButtons: Qt.NoButton
                        anchors.fill: parent
                        hoverEnabled: true
                    }
                }
                Rectangle {
                    id: date_dummy

                    anchors.fill: parent
                    color: "transparent"
                    visible: model.type === 1
                }

                BusyIndicator {
                    id: date_busy_indicator

                    anchors.centerIn: parent
                    width: editColumn
                    visible: model.type === "temp"
                }
            }

            Item {
                id: signItem
                anchors.right: mrpaItem.left
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                width: signColumn
                clip: true

                MouseArea {
                    id: sigStatusArea

                    anchors.fill: parent
                    hoverEnabled: true
                    enabled: signImage.visible

                    onClicked: {
                        rightSideBar.showState = RightSideBar.ShowState.Certs

                        fileTreeModel.getCertList(model.id)
                        console.warn(rightSideBar.sigCount)
                    }
                    onEntered: {
                        cursorShape = Qt.PointingHandCursor
                    }
                    onExited: {
                        cursorShape = Qt.ArrowCursor
                    }
                }

                Image {
                    id: signImage
                    anchors.centerIn: parent
                    source: getSignIcon(model.sig_color)
                    // visible: signStatusField.text !== ""
                    //          && !fileTreeModel.isDraft
                    visible: !fileTreeModel.isDraft && model.sig_color !== ""
                    width: height
                }

                // Text {
                //     id: signStatusField

                //     width: signColumn
                //     clip: true
                //     color: StyleSheet.font_color_extra
                //     font.pixelSize: column === 0 ? 14 : 10
                //     horizontalAlignment: Text.AlignHCenter
                //     text: model.sig_status
                //     visible: signStatusField.text !== ""
                //              && !fileTreeModel.isDraft
                // }
                Rectangle {
                    id: sign_status_dummy

                    anchors.fill: parent
                    width: signColumn
                    color: "transparent"
                    visible: //!signStatusField.visible
                             /*&&*/ !sign_busy_indicator.visible
                }
                BusyIndicator {
                    id: sign_busy_indicator

                    anchors.fill: parent
                    width: signColumn
                    visible: fileTreeModel.isDraft
                }
            }

            Item {
                id: mrpaItem
                anchors.right: deleteItem.left
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                width: mrpaColumn
                clip: true

                MouseArea {
                    id: mrpaStatusArea

                    anchors.fill: parent
                    hoverEnabled: true

                    onClicked: {
                        showMrpaList(fileTreeModel.getMrpaData(model.id))
                        rightSideBar.showState = RightSideBar.ShowState.Mrpa
                    }
                    onEntered: {
                        cursorShape = Qt.PointingHandCursor
                    }
                    onExited: {
                        cursorShape = Qt.ArrowCursor
                    }
                }

                Image {
                    id: mrpaImage
                    anchors.centerIn: parent
                    source: getSignIcon(model.mrpa_color)
                    // visible: signStatusField.text !== ""
                    //          && !fileTreeModel.isDraft
                    visible: !fileTreeModel.isDraft && model.mrpa_color !== ""
                    width: height
                }

                // Text {
                //     id: mrpaStatusField

                //     anchors.centerIn: parent
                //     width: mrpaColumn
                //     clip: true
                //     color: StyleSheet.font_color_extra
                //     font.pixelSize: column === 0 ? 14 : 10
                //     horizontalAlignment: Text.AlignHCenter
                //     text: model.mrpa_status
                //     visible: !fileTreeModel.isDraft
                // }
                Rectangle {
                    id: mrpa_status_dummy

                    anchors.fill: parent
                    width: mrpaColumn
                    visible: //!mrpaStatusField.visible
                             /*&&*/ !mrpa_busy_indicator.visible
                }
                BusyIndicator {
                    id: mrpa_busy_indicator

                    anchors.fill: parent
                    width: mrpaColumn
                    visible: fileTreeModel.isDraft
                }
            }

            Item {
                id: deleteItem
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                width: deleteColumn

                ToolButton {
                    id: deleteBtn

                    anchors.centerIn: parent
                    width: deleteColumn
                    icon.height: 20
                    icon.source: StyleSheet.trash_icon
                    icon.width: 20
                    visible: depth === 0
                    leftPadding: 0
                    rightPadding: 0
                    onClicked: {
                        console.warn("delete node res: " + fileTreeModel.deleteNode(
                                         model.full_path, row, model.uid,
                                         model.id))
                    }
                }
                Item {
                    id: dummyDelete

                    anchors.fill: parent
                    width: deleteColumn
                    visible: !deleteBtn.visible
                }
            }
        }
    }
    selectionModel: ItemSelectionModel {
        model: treeView.model
    }

    Connections {
        target: treeView.model
        function onSignDone(sign_result, sign_done) {
            console.warn("from treeView model:", JSON.stringify(sign_result))
            treeSignResultDialog.sign_result = JSON.parse(sign_result)
            treeSignResultDialog.sign_done = true
            enableSignButton()
            if (sign_done) {
                fileTreeModel.deleteTree()
                cleanWindow()
            }
        }
    }
}
