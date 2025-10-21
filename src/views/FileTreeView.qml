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
    signal disableSignButton
    signal errorOnSign(var err)

    property var state: []
    property var state_val: []

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
            Qt.callLater(function () {
                enableSignButton()
                treeSignResultDialog.close()
                errorMessageDialog.open()
            })
            throw new Error('Certificate data not found')
        }

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
        fileTreeModel.signTree(params)
    }

    delegate: Item {
        id: delegateTree

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
                return StyleSheet.green_sign_icon
            case "sig_red":
                return StyleSheet.red_sign_icon
            case "sig_mixed":
                return StyleSheet.medal_icon
            case "file_green":
                return StyleSheet.cell_icon_green
            case "file_red":
                return StyleSheet.cell_icon_red
            case "file_mixed":
                return StyleSheet.cell_icon_empty
            default:
                return ""
            }
        }

        function getMrpaIcon(color) {
            switch (color) {
            case "warning":
                return StyleSheet.warning_icon
            case "valid":
                return StyleSheet.file_text_green
            case "old":
                return StyleSheet.file_text_red
            case "invalid":
                return StyleSheet.file_text_icon
            case "file_green":
                return StyleSheet.cell_icon_green
            case "file_red":
                return StyleSheet.cell_icon_red
            case "file_mixed":
                return StyleSheet.cell_icon_empty
            default:
                return ""
            }
        }

        function restore() {
            if (!model.id) {
                return
            }

            let index_in_state = treeView.state.indexOf(model.id)
            if (index_in_state >= 0 && treeView.state_val[index_in_state]) {
                //console.warn("STATE: expand " + model.id)
                expand(row)
            }
        }

        onExpandedChanged: {
            //console.warn("id:" + model.id + "EXPANDED:" + isExpanded())
            if (model.id === undefined) {
                return
            }

            let index_in_state = treeView.state.indexOf(model.id)
            if (index_in_state >= 0) {
                treeView.state_val[index_in_state] = expanded
            } else {
                treeView.state.push(model.id)
                treeView.state_val.push(expanded)
            }
            // console.warn("STATEX:" + JSON.stringify(treeView.state))
            // console.warn("STATEX:" + JSON.stringify(treeView.state_val))
        }

        implicitHeight: nameField.implicitHeight * 1.6
        implicitWidth: treeView.width

        Item {
            property int id_model: model.id

            onId_modelChanged: {
                //console.warn("STATEX: call restore from child:" + model.id)
                parent.restore()
            }

            Component.onCompleted: {
                //console.warn("STATEX: call restore from child:" + model.id)
                parent.restore()
            }
        }

        Rectangle {
            id: background

            anchors.fill: parent
            border.color: row === treeView.currentRow ? "#3daee9" : "transparent"
            color: "transparent"
        }

        /// main item to represent each node of tree
        Item {
            anchors.fill: parent

            /// little indent
            Item {
                id: indent

                anchors.left: parent.left
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                width: indentation * depth
            }

            /// chevrone icon for expand/collapse action
            Item {
                id: indicatorItem
                anchors.left: indent.right
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                width: 30

                Image {
                    id: indicator

                    anchors.centerIn: parent
                    source: expanded ? StyleSheet.chevron_down : StyleSheet.chevron_right
                    sourceSize.height: expanded ? 10 : 15
                    sourceSize.width: expanded ? 20 : 10
                    visible: isTreeNode && hasChildren
                }

                TapHandler {
                    onTapped: eventPoint => {
                                  var ind
                                  try {
                                      ind = treeView.index(row, column)
                                  } catch (e) {
                                      let cell = treeView.cellAtPosition(
                                          eventPoint.pressPosition)

                                      // The user must have right-clicked an empty area; ignore it.
                                      if (cell.x === -1 && cell.y === -1) {
                                          return
                                      }
                                      ind = treeView.modelIndex(cell.y, cell.x)
                                  }
                                  treeView.selectionModel.setCurrentIndex(
                                      ind, ItemSelectionModel.NoUpdate)
                                  treeView.toggleExpanded(row)
                              }
                }

                HoverHandler {
                    cursorShape: Qt.PointingHandCursor
                }

                Item {
                    id: dummyItemIndicator

                    anchors.fill: parent
                    visible: !indicator.visible
                }
            }

            /// another little indent
            Item {
                id: treeIndent

                anchors.left: indicatorItem.right
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                width: indentation - 10
            }

            /// image based on node type
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

            /// name of file in node
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

                        acceptedButtons: Qt.RightButton
                        anchors.fill: parent
                        hoverEnabled: true

                        onClicked: mouse => {
                                       var ind
                                       try {
                                           ind = treeView.index(row, column)
                                       } catch (e) {
                                           let cell = treeView.cellAtPosition(
                                               Qt.point(mouse.x, mouse.y))

                                           // The user must have right-clicked an empty area; ignore it.
                                           if (cell.x === -1 && cell.y === -1) {
                                               return
                                           }

                                           ind = treeView.modelIndex(cell.y,
                                                                     cell.x)
                                       }
                                       treeView.selectionModel.setCurrentIndex(
                                           ind, ItemSelectionModel.NoUpdate)
                                       if (mouse.button === Qt.RightButton
                                           && model.type !== 1) {
                                           contextMenu.popup()
                                       }
                                   }

                        Menu {
                            id: contextMenu
                            MenuItem {
                                text: depth === 0 ? qsTr("Open file") : qsTr(
                                                        "Open copy")
                                onTriggered: {
                                    Qt.openUrlExternally(
                                                "file://" + model.full_path)
                                }
                            }
                        }
                    }
                }
            }

            /// size of file in node
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

            /// last edit date of file in node
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

            /// area with icon about file signature, can be clicked for additional info
            Item {
                id: signItem
                anchors.right: mrpaItem.left
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                width: signColumn
                clip: true
                ToolTip.delay: 500
                ToolTip.text: model.sig_status
                ToolTip.visible: sigStatusArea.hovered
                                 && model.sig_status !== ""

                Image {
                    id: signImage
                    anchors.centerIn: parent
                    source: getSignIcon(model.sig_color)
                    visible: !fileTreeModel.isDraft
                             && model.sig_color !== "empty"
                    width: height
                }

                TapHandler {
                    enabled: signImage.visible
                    onTapped: eventPoint => {
                                  var ind
                                  try {
                                      ind = treeView.index(row, column)
                                  } catch (e) {
                                      let cell = treeView.cellAtPosition(
                                          eventPoint.pressPosition)

                                      // The user must have right-clicked an empty area; ignore it.
                                      if (cell.x === -1 && cell.y === -1) {
                                          return
                                      }
                                      ind = treeView.modelIndex(cell.y, cell.x)
                                  }
                                  treeView.selectionModel.setCurrentIndex(
                                      ind, ItemSelectionModel.NoUpdate)
                                  rightSideBar.showState = RightSideBar.ShowState.Certs
                                  fileTreeModel.getCertList(model.id)
                              }
                }

                HoverHandler {
                    id: sigStatusArea
                    enabled: signImage.visible
                    cursorShape: Qt.PointingHandCursor
                }

                Rectangle {
                    id: sign_status_dummy

                    anchors.fill: parent
                    width: signColumn
                    color: "transparent"
                    visible: !sign_busy_indicator.visible
                }
                BusyIndicator {
                    id: sign_busy_indicator

                    anchors.fill: parent
                    width: signColumn
                    visible: fileTreeModel.isDraft
                }
            }

            /// area with icon about file MRPA, can be clicked for additional info
            Item {
                id: mrpaItem
                anchors.right: deleteItem.left
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                width: mrpaColumn
                clip: true
                ToolTip.delay: 500
                ToolTip.text: model.mrpa_status
                ToolTip.visible: mrpaStatusArea.hovered
                                 && model.mrpa_status !== ""

                Image {
                    id: mrpaImage
                    anchors.centerIn: parent
                    source: getMrpaIcon(model.mrpa_color)
                    visible: !fileTreeModel.isDraft
                             && model.mrpa_color !== "empty"
                    width: height
                }

                TapHandler {
                    enabled: mrpaImage.visible
                    onTapped: eventPoint => {
                                  var ind
                                  try {
                                      ind = treeView.index(row, column)
                                  } catch (e) {
                                      let cell = treeView.cellAtPosition(
                                          eventPoint.pressPosition)

                                      // The user must have right-clicked an empty area; ignore it.
                                      if (cell.x === -1 && cell.y === -1) {
                                          return
                                      }
                                      ind = treeView.modelIndex(cell.y, cell.x)
                                  }
                                  treeView.selectionModel.setCurrentIndex(
                                      ind, ItemSelectionModel.NoUpdate)
                                  showMrpaList(fileTreeModel.getMrpaData(
                                                   model.id))
                                  rightSideBar.showState = RightSideBar.ShowState.Mrpa
                              }
                }

                HoverHandler {
                    id: mrpaStatusArea
                    enabled: mrpaImage.visible
                    cursorShape: Qt.PointingHandCursor
                }

                Rectangle {
                    id: mrpa_status_dummy

                    anchors.fill: parent
                    width: mrpaColumn
                    color: "transparent"
                    visible: !mrpa_busy_indicator.visible
                }
                BusyIndicator {
                    id: mrpa_busy_indicator

                    anchors.fill: parent
                    width: mrpaColumn
                    visible: fileTreeModel.isDraft
                }
            }

            /// button to delete node from tree
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
                        rightSideBar.showState = RightSideBar.ShowState.Invisible
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
        model: fileTreeModel
    }

    /// we catch all clicks manually and push them further, since mouse clicks do not work in mate (qt6.4)
    MouseArea {
        anchors.fill: parent

        onClicked: mouse => {
                       let cell = treeView.cellAtPosition(Qt.point(mouse.x,
                                                                   mouse.y))
                       var ind
                       try {
                           ind = treeView.index(cell.y, cell.x)
                       } catch (e) {
                           // The user must have right-clicked an empty area; ignore it.
                           if (cell.x === -1 && cell.y === -1) {
                               return
                           }
                           ind = treeView.modelIndex(cell.x, cell.y)
                       }
                       treeView.selectionModel.setCurrentIndex(
                           ind, ItemSelectionModel.NoUpdate)
                       mouse.accepted = false
                   }

        onDoubleClicked: mouse => {
                             let cell = treeView.cellAtPosition(
                                 Qt.point(mouse.x, mouse.y))
                             var ind
                             try {
                                 ind = treeView.index(cell.y, cell.x)
                             } catch (e) {
                                 // The user must have right-clicked an empty area; ignore it.
                                 if (cell.x === -1 && cell.y === -1) {
                                     return
                                 }
                                 ind = treeView.modelIndex(cell.x, cell.y)
                             }
                             treeView.selectionModel.setCurrentIndex(
                                 ind, ItemSelectionModel.NoUpdate)
                             treeView.toggleExpanded(cell.y)
                             mouse.accepted = false
                         }
    }

    Connections {
        target: treeView.model
        function onSignDone(sign_result, sign_done) {
            enableSignButton()
            let res = JSON.parse(sign_result)
            const hasWarnings = (Array.isArray(res.warnings)
                                 && res.warnings.length > 0)

            if (hasWarnings) {
                const msg = Array.isArray(res.warnings) ? res.warnings.join(
                                                              "\n") : String(
                                                              res.warnings)
                errorOnSign(msg)
            }
            treeSignResultDialog.sign_result = res
            treeSignResultDialog.sign_done = true
            if (sign_done) {
                fileTreeModel.deleteTree()
                cleanWindow()
            }
        }

        function onDropState() {
            state = []
            state_val = []
        }

        function onTreeIsEmpty() {
            disableSignButton()
        }
    }
}
