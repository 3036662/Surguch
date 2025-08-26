import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt.labs.platform as QLP
import QtCore
import StyleSheet
import alt.pdfcsp.fontHelper
import alt.pdfcsp.eventFilterInstaller
import alt.pdfcsp.wheelFilter

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
                typeSwitch.checked = !stamp_json.create_from_image
                logoPath.text = stamp_json.img_path
                rubberStampText.text = stamp_json.stamp_text
                fontName.currentIndex = fontName.find(stamp_json.font_family)
                transparencySwitch.checked = stamp_json.bg_transparent
                borderSettings.border_width = stamp_json.border_width
                borderSettings.radius = stamp_json.border_radius
                rgbColorPicker.r = stamp_json.R
                rgbColorPicker.g = stamp_json.G
                rgbColorPicker.b = stamp_json.B
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
        typeSwitch.checked = true
        logoPath.text = ""
        rubberStampText.text = qsTr("Surguch")
        borderSettings.radius = 50
        borderSettings.border_width = 7
        transparencySwitch.checked = false
        rgbColorPicker.r = 50
        rgbColorPicker.g = 62
        rgbColorPicker.b = 168
    }

    function updatePreview() {
        let rubber_stamp_params = {
            "stamp_width": 400,
            "stamp_height": 400,
            "create_from_image": typeSwitch.checked ? 0 : 1,
            "img_path": logoPath.text,
            "border_width": borderSettings.border_width,
            "border_radius": borderSettings.radius,
            "text_color_red": rgbColorPicker.r,
            "text_color_green": rgbColorPicker.g,
            "text_color_blue": rgbColorPicker.b,
            "border_color_red": rgbColorPicker.r,
            "border_color_green": rgbColorPicker.g,
            "border_color_blue": rgbColorPicker.b,
            "bg_color_red": rgbColorPicker.r,
            "bg_color_green": rgbColorPicker.g,
            "bg_color_blue": rgbColorPicker.b,
            "font_family": fontName.currentText,
            "annotation_text": rubberStampText.text,
            "bg_transparent": transparencySwitch.checked ? 1 : 0,
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
        anchors.fill: parent

        width: root.width


        ColumnLayout {
            id: editColumn
            width: scrollView.width - scrollView.scrollBarWidth - 2 * StyleSheet.defaultPaddingH
            height: scrollView.availableHeight

            // top raw (label and close button)
            RowLayout {
                Layout.preferredWidth:  editColumn.width

                Label {
                    text: qsTr("Mark settings")
                    font.weight: Font.DemiBold
                    topPadding: 5
                    bottomPadding: 5
                    font.family: "Noto Sans"
                }

                Rectangle {
                    Layout.fillWidth: true
                }

                ToolButton {
                    flat: true
                    display: AbstractButton.IconOnly
                    icon.width: 20
                    icon.height: 20
                    leftPadding: 10
                    rightPadding: 10
                    topPadding: 10
                    bottomPadding: 10
                    font.family: "Noto Sans"
                    icon.source: StyleSheet.close_icon

                    onClicked: {
                        root.visible = false
                        resetData()
                    }
                }
            }

            // main raw
            RowLayout {
                id: mainRow

                Layout.preferredWidth: editColumn.width
                Layout.fillWidth: true

                spacing: 0

                // rubber stamp preview left panel
                RubberPreviewLeftPanel{
                     id: previewColumn
                     Layout.fillWidth: true

                     onSaveClicked:{
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
                         stamp_json["create_from_image"] = typeSwitch.checked ? 0 : 1
                         stamp_json["img_path"] = logoPath.text
                         stamp_json["stamp_text"] = rubberStampText.text
                         stamp_json["border_width"] = borderSettings.border_width
                         stamp_json["border_radius"] = borderSettings.radius
                         stamp_json["font_family"] = fontName.currentText
                         stamp_json["R"] = rgbColorPicker.r
                         stamp_json["G"] = rgbColorPicker.g
                         stamp_json["B"] = rgbColorPicker.b
                         stamp_json["bg_transparent"] = transparencySwitch.checked ? 1 : 0
                         const new_stamp_data = JSON.stringify(stamp_json)
                         console.warn(rubber_model.saveRubberStamps(
                                          new_stamp_data))
                         rubberStampEditor.visible = false
                         stamp_data = null
                     }

                     onDeleteClicked: {
                         if (rubber_model.deleteRubberStamps(
                                     root.stamp_id)) {
                             rubberStampEditor.visible = false
                             stamp_data = null
                         }
                     }
                }

                Item {
                    id: middleSpacer
                    width: 30
                }

                // right subcolomn
                ColumnLayout {
                    id: rightSubColumn

                    clip: true

                    Layout.fillWidth: false
                    Layout.preferredWidth: 355

                    // switch "generate from text"
                    RowLayout {
                        Layout.fillWidth: true

                        Text {
                            text: qsTr("Create from text")
                            Layout.fillWidth: true
                            color: StyleSheet.font_color_extra
                            font.family: "Noto Sans"
                        }

                        Rectangle {
                            Layout.fillWidth: true
                        }

                        SettingSwitch {
                            id: typeSwitch

                            topPadding: 5
                            bottomPadding: 5
                            rightPadding: 10

                            onToggled: {
                                updatePreview()
                            }
                        }
                    }

                    // generate from file
                    Text {
                        topPadding: 10
                        text: qsTr("Generate from file (*.png *.jpg *.jpeg *.bmp)")
                        bottomPadding: 5
                        font.family: "Noto Sans"
                        color: StyleSheet.font_color_extra
                        visible: !typeSwitch.checked
                    }

                    // logo path
                    TextArea {
                        id: logoPath
                        placeholderText: qsTr("Select a file")
                        visible: !typeSwitch.checked
                        Layout.fillWidth: true
                        font.family: "Noto Sans"
                        color: StyleSheet.font_color_extra

                        background: Rectangle {
                            border.color: StyleSheet.slider_border_color
                            color: "transparent"
                        }

                        onTextChanged: {
                            updatePreview()
                        }

                        MouseArea {
                            anchors.fill: parent

                            onClicked: {
                                imgFileDialog.open()
                            }
                        }
                    }

                    // simple text "Text"
                    Text {
                        text: qsTr("Text")
                        bottomPadding: 5
                        visible: typeSwitch.checked
                        font.family: "Noto Sans"
                        color: StyleSheet.font_color_extra
                    }

                    // text multistring
                    ScrollView {
                        Layout.fillWidth: true
                        Layout.maximumHeight: Math.min(
                                                  rubberStampText.implicitHeight,
                                                  font.pixelSize * 5 + 20)
                        visible: typeSwitch.checked

                        RSBTextArea {
                            id: rubberStampText
                            Layout.fillWidth: true
                            visible: typeSwitch.checked
                            placeholderText: qsTr("Enter text here")
                            selectByMouse: true
                            wrapMode: Text.WordWrap
                            placeholderTextColor: "grey"
                            font.family: "Noto Sans"
                            color: StyleSheet.font_color_extra

                            onTextChanged: {
                                updatePreview()
                            }
                        }
                        ScrollBar.vertical: ScrollBar {
                            Layout.fillWidth: true
                            anchors.right: parent.right
                            policy: (rubberStampText.lineCount
                                     > 5) ? ScrollBar.AsNeeded : ScrollBar.AlwaysOff
                        }
                    }

                    // simple text "Font"
                    Text {
                        text: qsTr("Font")
                        visible: typeSwitch.checked
                        bottomPadding: 5
                        font.family: "Noto Sans"
                        color: StyleSheet.font_color_extra
                    }

                    // font combo
                    ComboBox {
                        id: fontName
                        Layout.fillWidth: true
                        visible: typeSwitch.checked
                        model: fontHelper.cyrillicFamilies() //Qt.fontFamilies()
                        wheelEnabled: true

                        onActivated: {
                            updatePreview()
                        }

                        popup.onOpened: {
                            main_window_wheel_filter.dispactch_to_target = true
                        }
                        popup.onClosed: {
                            main_window_wheel_filter.dispactch_to_target = false
                        }
                        FontHelper {
                            id: fontHelper
                        }

                        Component.onCompleted: {
                            main_window_wheel_filter.setTargetForDispatch(
                                        fontName.popup.contentItem)
                            var index = model.indexOf("Noto Sans")
                            if (index >= 0) {
                                currentIndex = index
                            } else {
                                currentIndex = 0
                            }
                        }
                    }

                    // transparency switch with label
                    RowLayout {
                        Layout.fillWidth: true

                        Text {
                            text: qsTr("Transparency")
                            Layout.fillWidth: true
                            visible: typeSwitch.checked
                            color: StyleSheet.font_color_extra
                            font.family: "Noto Sans"
                            topPadding: 10
                            bottomPadding: 10
                        }

                        Rectangle {
                            Layout.fillWidth: true
                            visible: typeSwitch.checked
                        }

                        SettingSwitch {
                            id: transparencySwitch
                            visible: typeSwitch.checked

                            topPadding: 10
                            bottomPadding: 10
                            rightPadding: 10
                            rightInset: 10

                            onToggled: {
                                updatePreview()
                            }
                        }
                    }

                    // simple text "Color"
                    Text {
                        text: qsTr("Color")
                        visible: typeSwitch.checked
                        font.family: "Noto Sans"
                        color: StyleSheet.font_color_extra
                    }

                    // color sliders
                    RGBColorPicker{
                        id: rgbColorPicker

                        Layout.fillHeight: true
                        visible: typeSwitch.checked

                        onValueChanged: {
                            updatePreview();
                        }
                    }

                    // border settings
                    BorderSettings{
                        id: borderSettings

                        visible: typeSwitch.checked

                        onValueChanged: {
                             updatePreview();
                        }
                    }

                    Rectangle {
                        Layout.fillHeight: true
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
                logoPath.text = currentFile
            }
        }
    }
}
