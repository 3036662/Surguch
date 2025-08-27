import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import StyleSheet
import alt.pdfcsp.fontHelper
import alt.pdfcsp.eventFilterInstaller
import alt.pdfcsp.wheelFilter

ColumnLayout {
    id: root

    clip: true

    Layout.fillWidth: false
    Layout.preferredWidth: 355

    signal settingChanged();
    signal openFileSelectClicked();

    property alias typeSwitchChecked: typeSwitch.checked
    property alias logoPathText: logoPath.text
    property alias rubberStampText: rubberStampTextArea.text
    property alias fontNameIndex: fontName.currentIndex
    property alias fontNameVal:fontName.currentText
    property alias transparencySwitchChecked: transparencySwitch.checked
    property alias borderWidth: borderSettings.border_width
    property alias borderRadius: borderSettings.radius
    property alias r: rgbColorPicker.r
    property alias g: rgbColorPicker.g
    property alias b: rgbColorPicker.b

    function findFontIndexByName(name) {
        return fontName.find(name)
    }

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
                root.settingChanged()
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
            root.settingChanged()
        }

        MouseArea {
            anchors.fill: parent

            onClicked: {
                root.openFileSelectClicked();
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
        Layout.maximumHeight: Math.min(rubberStampTextArea.implicitHeight,
                                       font.pixelSize * 5 + 20)
        visible: typeSwitch.checked

        TextArea {
            id: rubberStampTextArea
            Layout.fillWidth: true
            visible: typeSwitch.checked
            placeholderText: qsTr("Enter text here")
            selectByMouse: true
            wrapMode: Text.WordWrap
            placeholderTextColor: "grey"
            font.family: "Noto Sans"
            color: StyleSheet.font_color_extra

            background: Rectangle {
                border.color: StyleSheet.slider_border_color
                color: StyleSheet.text_area_background
            }

            onTextChanged: {
                root.settingChanged()
            }
        }
        ScrollBar.vertical: ScrollBar {
            Layout.fillWidth: true
            anchors.right: parent.right
            policy: (rubberStampTextArea.lineCount > 5) ? ScrollBar.AsNeeded : ScrollBar.AlwaysOff
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
            root.settingChanged()
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
    TransparencySwitch {
        id: transparencySwitch

        Layout.fillWidth: true
        labelText: qsTr("Transparency")
        visible: typeSwitch.checked

        onToggled: {
            root.settingChanged()
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
    RGBColorPicker {
        id: rgbColorPicker

        Layout.fillHeight: true
        visible: typeSwitch.checked

        onValueChanged: {
            root.settingChanged()
        }
    }

    // border settings
    BorderSettings {
        id: borderSettings

        visible: typeSwitch.checked

        onValueChanged: {
            root.settingChanged()
        }
    }

    Rectangle {
        Layout.fillHeight: true
    }
}
