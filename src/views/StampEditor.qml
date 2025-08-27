import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts
import StyleSheet
import "stamp_editor_components" as StampComponents

Dialog {
    id: root


    property var profiles_model
    property var profile_data
    property var stamp_json
    property var stamp_data
    property int stamp_id: -1
    property bool editState: false

    // fill the form from stamp_data JSON string
    function updateStampForm() {
        if (stamp_data) {
            try {
                editState = true
                stamp_json = stamp_data
                stamp_id = stamp_json.id
                stampName.text = stamp_json.title
                transparencySwitch.checked = stamp_json.transparent
                borderSettings.border_width = stamp_json.border_width
                borderSettings.radius = stamp_json.border_radius
                rgbColorPicker.r = stamp_json.R
                rgbColorPicker.g = stamp_json.G
                rgbColorPicker.b = stamp_json.B
            } catch (e) {
                console.error("Error parsing JSON " + e.message)
            }
        } else {
            resetData()
        }
        stampPreview.profile_data = profile_data
        stampPreview.createPreview()
    }

    function resetData() {
        //console.warn("Reset data called")
        editState = false
        stamp_id = -1
        stampName.text = ""
        transparencySwitch.state = false
        borderSettings.border_width = 7
        borderSettings.radius = 50
        rgbColorPicker.r = 50
        rgbColorPicker.g = 62
        rgbColorPicker.b = 168
    }

    function updatePreview() {
        let stamp_params = {
            "stamp_name": stampName.text,
            "text_color_red": rgbColorPicker.r,
            "text_color_green": rgbColorPicker.g,
            "text_color_blue": rgbColorPicker.b,
            "border_color_red": rgbColorPicker.r,
            "border_color_green": rgbColorPicker.g,
            "border_color_blue": rgbColorPicker.b,
            "border_width": borderSettings.border_width,
            "border_radius": borderSettings.radius,
            "bg_transparent": transparencySwitch.checked ? 1 : 0
        }
        stampPreview.stamp_data = stamp_params
    }

    implicitWidth: 460
    implicitHeight: Math.min(715, parent.height * 0.9)
    padding: 0
    margins: 0
    visible: false
    modal: true
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2
    closePolicy: Popup.NoAutoClose

    contentItem: ScrollView {
        id: scrollView

        property bool scrollBarVisible: ScrollBar.vertical.visible
        property int scrollBarWidth: scrollBarVisible ? ScrollBar.vertical.width : 0


        clip: true
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

        ColumnLayout {
            id: editColumn
            width: scrollView.width - scrollView.scrollBarWidth - 2 * StyleSheet.defaultPaddingH
            height: scrollView.availableHeight

            // top raw (label and close button)
            StampComponents.TopLabelWithCloseButton {
                Layout.preferredWidth: editColumn.width

                labelText: qsTr("Stamp editor")

                onCloseClicked: {
                    stampEditor.visible = false
                    resetData()
                }
            }

            // stamp name
            StampComponents.StampNameInput{
                id: stampName

                labelText:  qsTr("Stamp name")
                placeholderText: qsTr("Enter stamp name")
            }

            // preview label
            Text {
                text: qsTr("Preview")
                bottomPadding: 5
                font.family: "Noto Sans"
                color: StyleSheet.font_color_extra
            }

            StampPreview {
                id: stampPreview

                Layout.fillWidth: true
                height: 175
            }

            // border settings
            StampComponents.BorderSettings {
                id: borderSettings

                onValueChanged: {
                    updatePreview()
                }
            }


            Text {
                text: qsTr("Stamp's color")
                font.family: "Noto Sans"
                color: StyleSheet.font_color_extra
            }


            // color settings
            StampComponents.RGBColorPicker {
                id: rgbColorPicker

                Layout.fillHeight: true

                onValueChanged: {
                    updatePreview()
                }
            }

            // transparency switch with label
            StampComponents.TransparencySwitch{
                id: transparencySwitch

                Layout.fillWidth: true
                labelText: qsTr("Transparency")

                onToggled: {
                    updatePreview()
                }
            }

            Rectangle {
                height: 8
            }

            Button {
                Layout.fillWidth: true
                text: qsTr("Save")
                font.family: "Noto Sans"
                display: AbstractButton.TextBesideIcon
                icon.source: StyleSheet.save_icon
                icon.width: 20
                icon.height: 20

                onClicked: {
                    if (stampName.text === "") {
                        stampName.forceActiveFocus()
                        return
                    }
                    if (stamp_id < 0 && !profiles_model.uniqueStampName(
                                stampName.text)) {
                        stampName.forceActiveFocus()
                        errorMessageDialog.text = qsTr(
                                    "Stamp with this name already exists")
                        errorMessageDialog.open()
                        return
                    }
                    stamp_json = {}
                    stamp_json["id"] = stamp_id
                    stamp_json["title"] = stampName.text
                    stamp_json["border_width"] = borderSettings.border_width
                    stamp_json["border_radius"] = borderSettings.radius
                    stamp_json["R"] = rgbColorPicker.r
                    stamp_json["G"] = rgbColorPicker.g
                    stamp_json["B"] = rgbColorPicker.b
                    stamp_json["transparent"] = transparencySwitch.checked ? 1 : 0
                    const new_stamp_data = JSON.stringify(stamp_json)
                    console.warn(profiles_model.saveStamp(new_stamp_data))
                    stampEditor.visible = false
                    stamp_data = null
                }
            }

            Button {
                id: deleteButton

                Layout.fillWidth: true
                text: qsTr("Delete")
                font.family: "Noto Sans"
                display: AbstractButton.TextBesideIcon
                icon.source: StyleSheet.trash_icon
                icon.width: 20
                icon.height: 20
                enabled: editState

                onClicked: {
                    if (profiles_model.deleteStamp(root.stamp_id)) {
                        profiles_model.updateProfiles(root.stamp_json.title)
                        stampEditor.visible = false
                        stamp_data = null
                    }
                }
            }
        }
    }
}
