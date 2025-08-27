import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import StyleSheet
import "../" as Surguch

// left subcolomn
// size of this column is implictly hardcoded (340)
ColumnLayout {
    id: previewColumn

    property alias rubberStampPreviewWidth: rubberStampPreview.width
    property alias rubberStampPreviewStampData: rubberStampPreview.stamp_data
    property alias stampNameText: stampName.text;
    property alias linkNameText:linkName.text
    property alias tagWidthValue: tagWidth.value

    function createPreview(){
         rubberStampPreview.createPreview();
    }

    signal saveClicked();
    signal deleteClicked();


    spacing:7

    // stamp name label
    Text {
        text: qsTr("Mark name")
        topPadding: 5
        bottomPadding: 5
        font.family: "Noto Sans"
        color: StyleSheet.font_color_extra
    }

    // stamp name text field
    Surguch.RSBTextArea {
        id: stampName
        Layout.fillWidth: true
        placeholderText: qsTr("Enter mark name")
        selectByMouse: true
        wrapMode: Text.WordWrap
        placeholderTextColor: "grey"
        font.family: "Noto Sans"
        color: StyleSheet.font_color_extra
        topPadding: 5
        bottomPadding: 5

        onTextChanged: {
            let validInput = stampName.text.match(/^S+$/)
            if (!validInput) {
                stampName.text = stampName.text.replace(/\s/g,
                                                        '')
                stampName.cursorPosition = stampName.text.length
            }
            if (stampName.text.length > 50) {
                stampName.text = stampName.text.slice(0, 50)
            }
        }
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

            Layout.preferredHeight:  280
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
    TextField{
        id: linkName
        Layout.fillWidth: true
        placeholderText: qsTr("Enter link here")
        placeholderTextColor: "grey"
        font.family: "Noto Sans"
        color: StyleSheet.font_color_extra

        onTextChanged: {
            updatePreview()
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
          saveClicked();
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
            deleteClicked();
        }
    }
    Item {
        Layout.fillHeight: true
    }
}
