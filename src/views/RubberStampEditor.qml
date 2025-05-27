import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt.labs.platform as QLP
import QtCore
import StyleSheet

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
                stampName.text = stamp_json.title
                linkName.text = stamp_json.stamp_link
                tagWidth.value = stamp_json.tag_width
                typeSwitch.checked = !stamp_json.create_from_image
                logoPath.text = stamp_json.img_path
                rubberStampText.text = stamp_json.stamp_text
                fontName.currentIndex = fontName.find(stamp_json.font_family)
                transparencySwitch.checked = stamp_json.bg_transparent
                borderWidth.value = stamp_json.border_width
                borderRadius.value = stamp_json.border_radius
                redColor.value = stamp_json.R
                greenColor.value = stamp_json.G
                blueColor.value = stamp_json.B
            } catch (e) {
                console.warn("Error parsing JSON " + e.message)
            }
        } else {
            resetData()
        }
        rubberStampPreview.createPreview()
    }

    function resetData() {
        edit_state = false
        stamp_id = -1
        stampName.text = ""
        linkName.text = ""
        tagWidth.value = 30
        typeSwitch.state = false
        logoPath.text = ""
        rubberStampText.text = ""
        borderRadius.value = 0
        borderWidth.value = 0
        transparencySwitch.state = false
        redColor.value = 0
        greenColor.value = 0
        blueColor.value = 0
    }

    function updatePreview() {
        let rubber_stamp_params = {
            "stamp_width": 400,
            "stamp_height": 400,
            "create_from_image": typeSwitch.checked ? 0 : 1,
            "img_path": logoPath.text,
            "border_width": borderWidth.value,
            "border_radius": borderRadius.value,
            "text_color_red": redColor.value,
            "text_color_green": greenColor.value,
            "text_color_blue": blueColor.value,
            "border_color_red": redColor.value,
            "border_color_green": greenColor.value,
            "border_color_blue": blueColor.value,
            "bg_color_red": redColor.value,
            "bg_color_green": greenColor.value,
            "bg_color_blue": blueColor.value,
            "font_family": fontName.currentText,
            "annotation_text": rubberStampText.text,
            "bg_transparent": transparencySwitch.checked ? 1 : 0,
            "annotation_width": rubberStampPreview.width
        }
        rubberStampPreview.stamp_data = rubber_stamp_params
    }


    width: 1000
    height: 800
    modal: true
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2
    closePolicy: Popup.NoAutoClose

    ColumnLayout {
        anchors.fill: parent

        RowLayout {
            Layout.fillWidth: true

            Label {
                text: qsTr("Tag settings")
            }

            Rectangle {
                Layout.fillWidth: true
            }

            ToolButton {
                flat: true
                display: AbstractButton.TextBesideIcon
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

        RowLayout {
            Layout.fillWidth: true

            ColumnLayout {
                Layout.preferredWidth: root.width / 2
                Layout.maximumWidth: root.width / 2

                Text {
                    text: qsTr("Tag name")
                    bottomPadding: 5
                    font.family: "Noto Sans"
                    color: StyleSheet.font_color_extra
                }

                TextArea {
                    id: stampName
                    Layout.fillWidth: true
                    placeholderText: qsTr("Enter tag name")
                    selectByMouse: true
                    wrapMode: Text.WordWrap
                    placeholderTextColor: "grey"
                    font.family: "Noto Sans"
                }

                Text {
                    text: qsTr("Preview")
                    bottomPadding: 5
                    font.family: "Noto Sans"
                    color: StyleSheet.font_color_extra
                }

                RubberPreview {
                    id: rubberStampPreview

                    //Layout.fillWidth: true
                    //Layout.fillHeight: true
                    width: 400
                    height: 400
                    //color: "white"
                    Component.onCompleted: {
                    }
                }

                Text {
                    text: qsTr("Link")
                    bottomPadding: 5
                    font.family: "Noto Sans"
                    color: StyleSheet.font_color_extra
                }

                TextArea {
                    id: linkName
                    Layout.fillWidth: true
                    placeholderText: qsTr("Enter link here")
                    selectByMouse: true
                    wrapMode: Text.WordWrap
                    placeholderTextColor: "grey"
                    font.family: "Noto Sans"
                }

                RowLayout {
                    Layout.fillWidth: true

                    Text {
                        text: qsTr("Default tag width, % from A4 ")
                        font.family: "Noto Sans"
                        color: StyleSheet.font_color_extra
                    }

                    Rectangle {
                        Layout.fillWidth: true
                    }

                    Text {
                        id: tagWidthText
                        text: tagWidth.value
                        font.family: "Noto Sans"
                        color: StyleSheet.font_color_extra
                    }
                }

                SettingSlider {
                    id: tagWidth
                    Layout.fillWidth: true
                    snapMode: Slider.SnapOnRelease
                    from: 0
                    to: 100
                    stepSize: 1
                }

                Button {
                    id: saveButton

                    Layout.fillWidth: true
                    text: qsTr("Save")
                    display: AbstractButton.TextBesideIcon
                    icon.source: StyleSheet.save_icon
                    icon.width: 20
                    icon.height: 20
                    onClicked: {
                        if (stamp_id < 0 && !rubber_model.uniqueStampName(stampName.text)) {
                            stampName.forceActiveFocus()
                            errorMessageDialog.text = qsTr(
                                "Stamp with this name already exists")
                            errorMessageDialog.open()
                            return
                        }
                        if (stampName.text === "") {
                            stampName.forceActiveFocus()
                            return
                        }
                        stamp_json = {}
                        stamp_json["id"] = stamp_id
                        stamp_json["title"] = stampName.text
                        stamp_json["stamp_link"] = linkName.text
                        stamp_json["tag_width"] = tagWidth.value
                        stamp_json["create_from_image"] = typeSwitch.checked ? 0 : 1
                        stamp_json["img_path"] = logoPath.text
                        stamp_json["stamp_text"] = rubberStampText.text
                        stamp_json["border_width"] = borderWidth.value
                        stamp_json["border_radius"] = borderRadius.value
                        stamp_json["font_family"] = fontName.currentText
                        stamp_json["R"] = redColor.value
                        stamp_json["G"] = greenColor.value
                        stamp_json["B"] = blueColor.value
                        stamp_json["bg_transparent"] = transparencySwitch.checked ? 1 : 0
                        const new_stamp_data = JSON.stringify(stamp_json)
                        console.warn(rubber_model.saveRubberStamps(new_stamp_data))
                        rubberStampEditor.visible = false
                        stamp_data = null
                    }
                }

                Button {
                    id: deleteButton

                    Layout.fillWidth: true
                    text: qsTr("Delete")
                    display: AbstractButton.TextBesideIcon
                    icon.source: StyleSheet.trash_icon
                    icon.width: 20
                    icon.height: 20
                    enabled: edit_state

                    onClicked: {
                        if (rubber_model.deleteRubberStamps(root.stamp_id)) {
                            rubberStampEditor.visible = false
                            stamp_data = null
                        }
                    }
                }
            }

            ColumnLayout {
                Layout.preferredWidth: root.width / 2
                Layout.maximumWidth: root.width / 2
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

                    Switch {
                        id: typeSwitch
                        scale: 1.5
                    }
                }

                Text {
                    topPadding: 10
                    text: qsTr("Generate from file (*.png *.jpg *.jpeg *.bmp)")
                    bottomPadding: 5
                    font.family: "Noto Sans"
                    color: StyleSheet.font_color_extra
                    visible: !typeSwitch.checked
                }

                TextArea {
                    id: logoPath
                    placeholderText: qsTr("Select a file")
                    visible: !typeSwitch.checked
                    Layout.fillWidth: true

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

                Text {
                    text: qsTr("Text")
                    bottomPadding: 5
                    visible: typeSwitch.checked
                    font.family: "Noto Sans"
                    color: StyleSheet.font_color_extra
                }

                TextArea {
                    id: rubberStampText
                    Layout.fillWidth: true
                    visible: typeSwitch.checked
                    placeholderText: qsTr("Enter text here")
                    selectByMouse: true
                    wrapMode: Text.WordWrap
                    placeholderTextColor: "grey"
                    font.family: "Noto Sans"

                    onTextChanged: {
                        updatePreview()
                    }
                }

                Text {
                    text: qsTr("Font")
                    visible: typeSwitch.checked
                    bottomPadding: 5
                    font.family: "Noto Sans"
                    color: StyleSheet.font_color_extra
                }

                ComboBox {
                    id: fontName
                    Layout.fillWidth: true
                    visible: typeSwitch.checked
                    model: Qt.fontFamilies()

                    onActivated: {
                        updatePreview()
                    }
                }

                RowLayout {
                    Layout.fillWidth: true

                    Text {
                        text: qsTr("Transparency")
                        Layout.fillWidth: true
                        visible: typeSwitch.checked
                        color: StyleSheet.font_color_extra
                        font.family: "Noto Sans"
                    }

                    Rectangle {
                        Layout.fillWidth: true
                        visible: typeSwitch.checked
                    }

                    Switch {
                        id: transparencySwitch
                        visible: typeSwitch.checked
                        scale: 1.5
                    }
                }

                Text {
                    text: qsTr("Color")
                    visible: typeSwitch.checked
                    font.family: "Noto Sans"
                    color: StyleSheet.font_color_extra
                }

                RowLayout {
                    Layout.fillWidth: true
                    visible: typeSwitch.checked

                    SettingSlider {
                        id: redColor

                        Layout.fillWidth: true
                        snapMode: Slider.SnapOnRelease
                        from: 0
                        to: 255
                        stepSize: 1
                        back_color: "red"

                        onValueChanged: {
                            updatePreview()
                        }
                    }

                    Text {
                        text: "R " + redColor.value
                        font.family: "Noto Sans"
                        color: StyleSheet.font_color_extra
                    }
                }

                RowLayout {
                    Layout.fillWidth: true
                    visible: typeSwitch.checked

                    SettingSlider {
                        id: greenColor

                        Layout.fillWidth: true
                        snapMode: Slider.SnapOnRelease
                        from: 0
                        to: 255
                        stepSize: 1
                        back_color: "green"

                        onValueChanged: {
                            updatePreview()
                        }
                    }

                    Text {
                        text: "G " + greenColor.value
                        font.family: "Noto Sans"
                        color: StyleSheet.font_color_extra
                    }
                }

                RowLayout {
                    Layout.fillWidth: true
                    visible: typeSwitch.checked

                    SettingSlider {
                        id: blueColor

                        Layout.fillWidth: true
                        snapMode: Slider.SnapOnRelease
                        from: 0
                        to: 255
                        stepSize: 1
                        back_color: "blue"

                        onValueChanged: {
                            updatePreview()
                        }
                    }

                    Text {
                        text: "B " + blueColor.value
                        font.family: "Noto Sans"
                        color: StyleSheet.font_color_extra
                    }
                }

                RowLayout {
                    Layout.fillWidth: true
                    visible: typeSwitch.checked

                    Text {
                        text: qsTr("Stamp border width: ")
                        font.family: "Noto Sans"
                        color: StyleSheet.font_color_extra
                    }

                    Rectangle {
                        Layout.fillWidth: true
                    }

                    Text {
                        id: borderWidthText
                        text: borderWidth.value
                        font.family: "Noto Sans"
                        color: StyleSheet.font_color_extra
                    }
                }

                SettingSlider {
                    id: borderWidth
                    Layout.fillWidth: true
                    visible: typeSwitch.checked
                    snapMode: Slider.SnapOnRelease
                    from: 0
                    to: 20
                    stepSize: 1

                    onValueChanged: {
                        updatePreview()
                    }
                }

                RowLayout {
                    Layout.fillWidth: true
                    visible: typeSwitch.checked

                    Text {
                        text: qsTr("Stamp border radius: ")
                        font.family: "Noto Sans"
                        color: StyleSheet.font_color_extra
                    }

                    Rectangle {
                        Layout.fillWidth: true
                    }

                    Text {
                        id: borderRadiusText
                        text: borderRadius.value
                        font.family: "Noto Sans"
                        color: StyleSheet.font_color_extra
                    }
                }

                SettingSlider {
                    id: borderRadius
                    Layout.fillWidth: true
                    visible: typeSwitch.checked
                    snapMode: Slider.SnapOnRelease
                    from: 0
                    to: 70
                    stepSize: 1

                    onValueChanged: {
                        updatePreview()
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
        folder: StandardPaths.writableLocation(StandardPaths.DocumentsLocation)
        onAccepted: {
            logoPath.text = currentFile
        }
    }
}
