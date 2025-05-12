import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts
import StyleSheet

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
                stamp_json = stamp_data
                stamp_id = stamp_json.id
                stampName.text = stamp_json.title
                transparencySwitch.checked = stamp_json.transparent
                borderWidth.value = stamp_json.border_width
                borderRadius.value = stamp_json.border_radius
                redColor.value = stamp_json.R
                greenColor.value = stamp_json.G
                blueColor.value = stamp_json.B
            } catch (e) {
                console.error("Error parsing JSON " + e.message)
            }
        } else {
            editState = false
            stamp_id = -1
            stampName.text = ""
            transparencySwitch.state = false
            borderWidth.value = 7
            borderRadius.value = 50
            redColor.value = 50
            greenColor.value = 62
            blueColor.value = 168
        }
        //console.warn("edit" + profile_data)
        stampPreview.profile_data = profile_data
        stampPreview.createPreview()
    }

    function updatePreview(){
        let stamp_params = {
            "text_color_red": redColor.value,
            "text_color_green": greenColor.value,
            "text_color_blue": blueColor.value,
            "border_color_red": redColor.value,
            "border_color_green": greenColor.value,
            "border_color_blue": blueColor.value,
            "border_width": borderWidth.value,
            "border_radius": borderRadius.value,
            "bg_transparent": transparencySwitch.checked ? 1 : 0
        }
        stampPreview.stamp_data = stamp_params
    }

    width: 400
    height: 700
    visible: false
    modal: true
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2
    closePolicy: Popup.NoAutoClose

    ColumnLayout {
        id: editColumn
        anchors.fill: parent

        RowLayout {
            width: root.width
            Layout.fillWidth: true

            Label {
                text: qsTr("Stamp editor")
            }

            Rectangle {
                Layout.fillWidth: true
            }

            ToolButton{
                flat:true
                display:AbstractButton.TextBesideIcon
                icon.width: 30
                icon.height: 30
                leftPadding: 10
                rightPadding: 10
                topPadding: 10
                bottomPadding: 10
                font.family: "Noto Sans"
                icon.source: StyleSheet.close_icon

                onClicked: {
                    stampEditor.visible = false
                }
            }
        }

        Text {
            text: qsTr("Stamp name")
            bottomPadding: 5
            font.family: "Noto Sans"
            color: StyleSheet.font_color_extra
        }

        TextArea{
            id: stampName
            Layout.fillWidth: true
            placeholderText: qsTr("Enter stamp name")
            selectByMouse: true
            width: parent.width
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

        StampPreview {
            id: stampPreview

            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        RowLayout {
            width: root.width

            Text {
                text: qsTr("Transparency")
                Layout.fillWidth: true
                color: StyleSheet.font_color_extra
                font.family: "Noto Sans"
            }

            Rectangle {
                Layout.fillWidth: true
            }

            Switch {
                id: transparencySwitch
            }
        }

        Text {
            text: qsTr("Stamp border width: ")
            font.family: "Noto Sans"
            color: StyleSheet.font_color_extra
        }

        Slider {
            id: borderWidth
            Layout.fillWidth: true
            snapMode: Slider.SnapOnRelease
            from: 0
            to: 20
            stepSize: 1

            onValueChanged: {
                updatePreview()
            }
        }

        Text {
            id: borderWidthText
            text: borderWidth.value
            font.family: "Noto Sans"
            color: StyleSheet.font_color_extra
        }

        Text {
            text: qsTr("Stamp border radius: ")
            font.family: "Noto Sans"
            color: StyleSheet.font_color_extra
        }

        Slider {
            id: borderRadius
            Layout.fillWidth: true
            snapMode: Slider.SnapOnRelease
            from: 0
            to: 70
            stepSize: 1

            onValueChanged: {
                updatePreview()
            }
        }

        Text {
            id: borderRadiusText
            text: qsTr("Stamp border radius: ") + borderRadius.value
            font.family: "Noto Sans"
            color: StyleSheet.font_color_extra
        }

        Text {
            text: qsTr("Stamp's color")
            font.family: "Noto Sans"
            color: StyleSheet.font_color_extra
        }

        RowLayout {
            width: root.width

            ColorSlider {
                id: redColor

                Layout.fillWidth: true
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
            width: root.width
            ColorSlider {
                id: greenColor

                Layout.fillWidth: true
                back_color: "blue"

                onValueChanged: {
                    updatePreview()
                }

            }

            Text {
                text: "G" + greenColor.value
                font.family: "Noto Sans"
                color: StyleSheet.font_color_extra
            }
        }

        RowLayout {
            width: root.width

            ColorSlider {
                id: blueColor

                Layout.fillWidth: true
                back_color: "green"

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

        Button {
            Layout.fillWidth: true
            text: qsTr("Save")

            onClicked: {
                console.warn(transparencySwitch.checked)
                if (stamp_id<0 &&
                    !profiles_model.uniqueStampName(stampName.text)){
                    stampName.forceActiveFocus()
                    errorMessageDialog.text=qsTr("Stamp with this name already exists");
                    errorMessageDialog.open();
                    return
                }
                if (stampName.text === "") {
                    stampName.forceActiveFocus()
                    return
                }
                stamp_json = {}
                stamp_json["id"] = stamp_id
                stamp_json["title"] = stampName.text
                stamp_json["border_width"] = borderWidth.value
                stamp_json["border_radius"] = borderRadius.value
                stamp_json["R"] = redColor.value
                stamp_json["G"] = greenColor.value
                stamp_json["B"] = blueColor.value
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
