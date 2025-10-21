import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import StyleSheet
import "../" as Surguch

// left sub column
// size of this column is implicitly hardcoded (340)
ColumnLayout {
    id: previewColumn

    readonly property alias stampNameText: stampName.text
    readonly property alias linkNameText: linkName.text
    readonly property alias tagWidth: tagWidth.value

    // if true createPreview() call will not be triggered
    // prevents multiple triggers when many settings are changed simultaneously in one step.
    property bool ignore_changes: false

    signal saveClicked
    signal deleteClicked

    function createPreview() {
        rubberStampPreview.createPreview()
    }

    // update stamp settings  for render
    function setRenderData(params) {
        rubberStampPreview.stamp_json = params
    }

    function focusOnName() {
        stampName.forceActiveFocus()
    }

    // ---------------------
    // setter functions prevent from triggering the createPreview() function
    // on each change, allow making many changes in one stamp
    function setStampName(val) {
        ignore_changes = true
        stampName.text = val
        ignore_changes = false
    }

    function setLink(val) {
        ignore_changes = true
        linkName.text = val
        ignore_changes = false
    }

    function setTagWidth(val) {
        ignore_changes = true
        tagWidth.value = val
        ignore_changes = false
    }

    function reset() {
        ignore_changes = true
        stampName.text = ""
        linkName.text = ""
        tagWidth.value = 30
        ignore_changes = false
    }

    spacing: 7

    // stamp name
    StampNameInput {
        id: stampName

        labelText: qsTr("Mark name")
        placeholderText: qsTr("Enter mark name")
    }

    // preview label
    Text {
        text: qsTr("Preview")
        bottomPadding: 5
        font.family: "Noto Sans"
        color: StyleSheet.font_color_extra
    }

    // preview item
    RubberPreview {
        id: rubberStampPreview

        Layout.preferredHeight: 280
        Layout.fillWidth: true

        imageWidth: 340
        imageHeight: 280
    }

    // link edit label
    Text {
        text: qsTr("Link")
        bottomPadding: 5
        font.family: "Noto Sans"
        color: StyleSheet.font_color_extra
    }

    // link edit field
    TextField {
        id: linkName
        Layout.fillWidth: true
        placeholderText: qsTr("Enter link here")
        placeholderTextColor: "grey"
        font.family: "Noto Sans"
        color: StyleSheet.font_color_extra

        onTextChanged: {
            if (!ignore_changes) {
                updatePreview()
            }
        }
    }

    // stamp width slider label
    RowLayout {
        Layout.fillWidth: true

        Text {
            text: qsTr("Default mark width, % from A4 ")
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

    // stamp width slider
    SettingSlider {
        id: tagWidth
        Layout.fillWidth: true
        snapMode: Slider.SnapOnRelease
        from: 1
        to: 99
        stepSize: 1
    }

    Button {
        id: saveButton

        Layout.fillWidth: true
        text: qsTr("Save")
        font.family: "Noto Sans"
        display: AbstractButton.TextBesideIcon
        icon.source: StyleSheet.save_icon
        icon.width: 20
        icon.height: 20
        onClicked: {
            saveClicked()
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
        enabled: edit_state

        onClicked: {
            deleteClicked()
        }
    }
    Item {
        Layout.fillHeight: true
    }
}
