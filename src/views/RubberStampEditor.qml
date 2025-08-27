import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt.labs.platform as QLP
import QtCore
import StyleSheet
import alt.pdfcsp.fontHelper
import alt.pdfcsp.eventFilterInstaller
import alt.pdfcsp.wheelFilter
import "stamp_editor_components" as StampComponents

Dialog {
    id: root

    property var stamp_json
    property var stamp_data
    property var rubber_model
    property int stamp_id: -1
    property bool edit_state: true

    // fill form with data from JSON
    function updateRubberStampForm() {
        if (stamp_data) {
            try {
                edit_state = true
                stamp_json = JSON.parse(stamp_data)
                stamp_id = stamp_json.id
                previewColumn.stampNameText = stamp_json.title
                previewColumn.linkNameText = stamp_json.stamp_link
                previewColumn.tagWidthValue = stamp_json.tag_width
                rightSubColumn.typeSwitchChecked = !stamp_json.create_from_image
                rightSubColumn.logoPathText = stamp_json.img_path
                rightSubColumn.rubberStampText = stamp_json.stamp_text
                rightSubColumn.fontNameIndex = rightSubColumn.findFontIndexByName(
                            stamp_json.font_family)
                rightSubColumn.transparencySwitchChecked = stamp_json.bg_transparent
                rightSubColumn.borderWidth = stamp_json.border_width
                rightSubColumn.borderRadius = stamp_json.border_radius
                rightSubColumn.r = stamp_json.R
                rightSubColumn.g = stamp_json.G
                rightSubColumn.b = stamp_json.B
            } catch (e) {
                console.warn("Error parsing JSON " + e.message)
            }
        } else {
            resetData()
        }
        previewColumn.createPreview()
    }

    function resetData() {
        edit_state = false
        stamp_id = -1
        previewColumn.stampNameText = ""
        previewColumn.linkNameText = ""
        previewColumn.tagWidthValue = 30
        rightSubColumn.typeSwitchChecked = true
        rightSubColumn.logoPathText = ""
        rightSubColumn.rubberStampText = qsTr("Surguch")
        rightSubColumn.borderRadius = 50
        rightSubColumn.borderWidth = 7
        rightSubColumn.transparencySwitchChecked = false
        rightSubColumn.r = 50
        rightSubColumn.g = 62
        rightSubColumn.b = 168
    }

    function updatePreview() {
        let rubber_stamp_params = {
            "stamp_width": 400,
            "stamp_height": 400,
            "create_from_image": rightSubColumn.typeSwitchChecked ? 0 : 1,
            "img_path": rightSubColumn.logoPathText,
            "border_width": rightSubColumn.borderWidth,
            "border_radius": rightSubColumn.borderRadius,
            "text_color_red": rightSubColumn.r,
            "text_color_green": rightSubColumn.g,
            "text_color_blue": rightSubColumn.b,
            "border_color_red": rightSubColumn.r,
            "border_color_green": rightSubColumn.g,
            "border_color_blue": rightSubColumn.b,
            "bg_color_red": rightSubColumn.r,
            "bg_color_green": rightSubColumn.g,
            "bg_color_blue": rightSubColumn.b,
            "font_family": rightSubColumn.fontNameVal,
            "annotation_text": rightSubColumn.rubberStampText,
            "bg_transparent": rightSubColumn.transparencySwitchChecked ? 1 : 0,
            "annotation_width": previewColumn.rubberStampPreviewWidth
        }
        previewColumn.rubberStampPreviewStampData = rubber_stamp_params
    }

    function prependInternetProtocol(link) {
        const trimmedLink = link.trim()
        if (!trimmedLink) {
            return trimmedLink
        }

        const acceptableProtocols = ["http", "https"]
        const isValidForm = acceptableProtocols.some(
                              prot => trimmedLink.startsWith(`${prot}://`))

        return isValidForm ? trimmedLink : "http://" + trimmedLink
    }

    implicitWidth: 780
    implicitHeight: Math.min(670, parent.height * 0.85)
    modal: true
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2
    closePolicy: Popup.NoAutoClose
    padding: 0
    margins: 0

    // main item
    contentItem: ScrollView {
        id: scrollView

        property bool scrollBarVisible: ScrollBar.vertical.visible
        property int scrollBarWidth: scrollBarVisible ? ScrollBar.vertical.width : 0

        clip: true
        ScrollBar.horizontal.policy: ScrollBar.AsNeeded
        ScrollBar.vertical.policy: ScrollBar.AsNeeded

        topPadding: StyleSheet.defaultPaddingV
        bottomPadding: StyleSheet.defaultPaddingV
        leftPadding: StyleSheet.defaultPaddingH
        rightPadding: StyleSheet.defaultPaddingH
        anchors.topMargin: 0
        anchors.bottomMargin: 0
        anchors.leftMargin: 0
        anchors.rightMargin: 0
        anchors.fill: parent

        width: root.width

        ColumnLayout {
            id: editColumn
            width: scrollView.width - scrollView.scrollBarWidth - 2 * StyleSheet.defaultPaddingH
            height: scrollView.availableHeight

            // top raw (label and close button)
            StampComponents.TopLabelWithCloseButton {
                Layout.preferredWidth: editColumn.width

                labelText: qsTr("Mark settings")

                onCloseClicked: {
                    root.visible = false
                    resetData()
                }
            }

            // main raw
            RowLayout {
                id: mainRow

                Layout.preferredWidth: editColumn.width
                Layout.fillWidth: true

                spacing: 0

                // rubber stamp preview left panel
                StampComponents.RubberPreviewLeftPanel {
                    id: previewColumn
                    Layout.fillWidth: true

                    onSaveClicked: {
                        if (previewColumn.stampNameText === "") {
                            stampName.forceActiveFocus()
                            return
                        }
                        if (stamp_id < 0 && !rubber_model.uniqueStampName(
                                    previewColumn.stampNameText)) {
                            stampName.forceActiveFocus()
                            errorMessageDialog.text = qsTr(
                                        "Stamp with this name already exists")
                            errorMessageDialog.open()
                            return
                        }
                        stamp_json = {}
                        stamp_json["id"] = stamp_id
                        stamp_json["title"] = previewColumn.stampNameText
                        stamp_json["stamp_link"] = prependInternetProtocol(
                                    previewColumn.linkNameText)
                        stamp_json["tag_width"] = previewColumn.tagWidthValue
                        stamp_json["create_from_image"] = rightSubColumn.typeSwitchChecked ? 0 : 1
                        stamp_json["img_path"] = rightSubColumn.logoPathText
                        stamp_json["stamp_text"] = rightSubColumn.rubberStampText
                        stamp_json["border_width"] = rightSubColumn.borderWidth
                        stamp_json["border_radius"] = rightSubColumn.borderRadius
                        stamp_json["font_family"] = rightSubColumn.fontNameVal
                        stamp_json["R"] = rightSubColumn.r
                        stamp_json["G"] = rightSubColumn.g
                        stamp_json["B"] = rightSubColumn.b
                        stamp_json["bg_transparent"]
                                = rightSubColumn.transparencySwitchChecked ? 1 : 0
                        const new_stamp_data = JSON.stringify(stamp_json)
                        console.warn(rubber_model.saveRubberStamps(
                                         new_stamp_data))
                        rubberStampEditor.visible = false
                        stamp_data = null
                    }

                    onDeleteClicked: {
                        if (rubber_model.deleteRubberStamps(root.stamp_id)) {
                            rubberStampEditor.visible = false
                            stamp_data = null
                        }
                    }
                }

                Item {
                    id: middleSpacer
                    width: 30
                }

                // ------------------
                // right subcolomn
                StampComponents.RubberPreviewRightPanel {
                    id: rightSubColumn

                    onSettingChanged:{
                         updatePreview()
                    }

                    onOpenFileSelectClicked:{
                         imgFileDialog.open()
                    }
                }
            }
        }

        QLP.FileDialog {
            id: imgFileDialog
            fileMode: QLP.FileDialog.OpenFile
            nameFilters: ["Image files (*.png *.jpg *.jpeg *.bmp)"]
            options: QLP.FileDialog.ReadOnly
            folder: StandardPaths.writableLocation(
                        StandardPaths.DocumentsLocation)
            onAccepted: {
                rightSubColumn.logoPathText = currentFile
            }
        }
    }
}
