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

    // these fields are set from RubberStampItem.qml
    property var stamp_data // stamp data string;
    property var rubber_model // reference to cpp model;
    property int stamp_id: -1
    property bool edit_state: true

    // private properties
    Item{
     id: private_data
     visible:false
     property var stamp_json  // stamp data JSON
    }

    // fill form with data from JSON
    function updateRubberStampForm() {
        //console.warn("updateRubberStampForm: "+ stamp_data)
        if (stamp_data) {
            try {
                edit_state = true
                private_data.stamp_json = JSON.parse(stamp_data)
                stamp_id = private_data.stamp_json.id
                // update left column
                previewColumn.setStampName(private_data.stamp_json.title)
                previewColumn.setLink(private_data.stamp_json.stamp_link)
                previewColumn.setTagWidth(private_data.stamp_json.tag_width)
                // update right column
                rightSubColumn.update(private_data.stamp_json)
            } catch (e) {

                console.warn("[updateRubberStampForm] Error parsing JSON " + e.message)
            }
        } else {
            resetData()
        }
        updatePreview()
    }

    // reset all stamp settings
    function resetData() {
        edit_state = false
        stamp_id = -1
        previewColumn.reset()
        rightSubColumn.reset()
    }

    function updatePreview() {
        let params = rightSubColumn.getParams()
        // TODO(Oleg) Harcoded?
        params["stamp_width"] = 400
        params["stamp_height"] = 400
        params["annotation_width"] = previewColumn.tagWidth
        previewColumn.setRenderData(params)
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
                            previewColumn.focusOnName()
                            return
                        }
                        if (stamp_id < 0 && !rubber_model.uniqueStampName(
                                    previewColumn.stampNameText)) {
                            previewColumn.focusOnName()
                            errorMessageDialog.text = qsTr(
                                        "Stamp with this name already exists")
                            errorMessageDialog.open()
                            return
                        }
                        private_data.stamp_json = rightSubColumn.getParams()
                        private_data.stamp_json["id"] = stamp_id
                        private_data.stamp_json["title"] = previewColumn.stampNameText
                        private_data.stamp_json["stamp_link"] = prependInternetProtocol(
                                    previewColumn.linkNameText)
                        private_data.stamp_json["tag_width"] = previewColumn.tagWidth
                        const new_stamp_data = JSON.stringify(private_data.stamp_json)
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

                    onSettingChanged: {
                        updatePreview()
                    }

                    onOpenFileSelectClicked: {
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
                rightSubColumn.setLogo(currentFile)
            }
        }
    }
}
