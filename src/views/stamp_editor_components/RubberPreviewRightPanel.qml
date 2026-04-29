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

    signal settingChanged
    signal openFileSelectClicked

    // if true settingChanged signal will not be triggered
    // prevents multiple event triggers when many settings are changed simultaneously in one step.
    property bool ignore_changes: true

    function update(stamp_json) {
        ignore_changes = true
        typeSwitch.checked = !stamp_json.create_from_image
        logoPath.text = stamp_json.img_path
        rubberStampTextArea.text = stamp_json.stamp_text
        fontName.currentIndex = fontName.find(stamp_json.font_family)
        transparencySwitch.checked = stamp_json.bg_transparent
        borderSettings.border_width = stamp_json.border_width
        borderSettings.radius = stamp_json.border_radius
        rgbColorPicker.r = stamp_json.R
        rgbColorPicker.g = stamp_json.G
        rgbColorPicker.b = stamp_json.B
        ignore_changes = false
    }

    function reset() {
        ignore_changes = true
        typeSwitch.checked = true
        logoPath.text = ""
        rubberStampTextArea.text = qsTr("Surguch")
        borderSettings.radius = 50
        borderSettings.border_width = 7
        transparencySwitch.checked = false
        rgbColorPicker.r = 50
        rgbColorPicker.g = 62
        rgbColorPicker.b = 168
        ignore_changes = false
    }

    // TODO(Oleg) Any ideas why:


    /*
      a) in some cases, you need
         annotation_text,in other cases stamp_text

      b) in some cases colors are saved as:
          "R","G","B"
          in other cases "border_color_red","border_color_green","bg_color_blue"
    */
    function getParams() {
        let rubber_stamp_params = {
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
            "annotation_text": rubberStampTextArea.text,
            "stamp_text": rubberStampTextArea.text,
            "bg_transparent": transparencySwitch.checked ? 1 : 0,
            "R": rgbColorPicker.r,
            "G": rgbColorPicker.g,
            "B": rgbColorPicker.b
        }
        return rubber_stamp_params
    }

    function setLogo(path) {
        ignore_changes = false
        logoPath.text = path
    }

    // switch "generate from text"
    RowLayout {
        Layout.fillWidth: true

        Text {
            text: qsTr("Create from text")
            Layout.fillWidth: true
            color: StyleSheet.font_color_extra
            font.family: StyleSheet.defaultFontFamily
            font.pointSize: StyleSheet.defaultTextPointSize
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
                if (!ignore_changes) {
                    root.settingChanged()
                }
            }
        }
    }

    // generate from file
    Text {
        id: filesExtensionsText
        topPadding: 10
        text: qsTr("Generate from file (*.png *.jpg *.jpeg *.bmp)")
        bottomPadding: 5
        font.family: StyleSheet.defaultFontFamily
        font.pointSize: StyleSheet.defaultTextPointSize

        color: StyleSheet.font_color_extra
        visible: !typeSwitch.checked
    }

    // logo path
    TextArea {
        id: logoPath
        placeholderText: qsTr("Select a file")
        visible: !typeSwitch.checked
        Layout.fillWidth: true
        font.family: StyleSheet.defaultFontFamily
        font.pointSize: StyleSheet.defaultTextPointSize
        color: StyleSheet.font_color_extra

        background: Rectangle {
            border.color: StyleSheet.slider_border_color
            color: "transparent"
            radius: 4
        }

        onTextChanged: {
            if (!ignore_changes) {
                root.settingChanged()
            }
        }

        MouseArea {
            anchors.fill: parent

            onClicked: {
                root.openFileSelectClicked()
            }
        }
    }

    // simple text "Text"
    Text {
        text: qsTr("Text")
        bottomPadding: 5
        visible: typeSwitch.checked
        font.family: StyleSheet.defaultFontFamily
        font.pointSize: StyleSheet.defaultTextPointSize
        color: StyleSheet.font_color_extra
    }

    // text multi string
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
            font.family: StyleSheet.defaultFontFamily
            font.pointSize: StyleSheet.defaultTextPointSize
            color: StyleSheet.font_color_extra

            background: Rectangle {
                border.color: StyleSheet.slider_border_color
                color: StyleSheet.text_area_background
                radius: 4
            }

            onTextChanged: {
                if (!ignore_changes) {
                    root.settingChanged()
                }
            }
        }
        ScrollBar.vertical: ScrollBar {
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.rightMargin: 10
            width: 8
            policy: (rubberStampTextArea.lineCount > 5) ? ScrollBar.AlwaysOn : ScrollBar.AlwaysOff

            contentItem: Rectangle {
                implicitWidth: 6
                radius: width / 2
                color: StyleSheet.slider_fill_color
                border.width: 2
                border.color: StyleSheet.slider_border_color
            }

            background: Rectangle {
                color: "transparent"
            }
        }
    }

    // simple text "Font"
    Text {
        text: qsTr("Font")
        visible: typeSwitch.checked
        bottomPadding: 5
        font.family: StyleSheet.defaultFontFamily
        font.pointSize: StyleSheet.defaultTextPointSize
        color: StyleSheet.font_color_extra
    }

    // font combo
    ComboBox {
        id: fontName
        Layout.fillWidth: true
        visible: typeSwitch.checked
        model: fontHelper.cyrillicFamilies() //Qt.fontFamilies()
        wheelEnabled: true

        font.family: StyleSheet.defaultFontFamily
        font.pointSize: StyleSheet.defaultTextPointSize

        onActivated: {
            if (!ignore_changes) {
                root.settingChanged()
            }
        }

        popup.onOpened: {
            main_window_wheel_filter.dispatch_to_target = true
        }
        popup.onClosed: {
            main_window_wheel_filter.dispatch_to_target = false
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
            if (!ignore_changes) {
                root.settingChanged()
            }
        }
    }

    // simple text "Color"
    Text {
        text: qsTr("Color")
        visible: typeSwitch.checked
        font.family: StyleSheet.defaultFontFamily
        font.pointSize: StyleSheet.defaultTextPointSize
        color: StyleSheet.font_color_extra
    }

    // color sliders
    RGBColorPicker {
        id: rgbColorPicker

        Layout.fillHeight: true
        visible: typeSwitch.checked

        onValueChanged: {
            if (!ignore_changes) {
                root.settingChanged()
            }
        }
    }

    // border settings
    BorderSettings {
        id: borderSettings

        visible: typeSwitch.checked

        onValueChanged: {
            if (!ignore_changes) {
                root.settingChanged()
            }
        }
    }

    Rectangle {
        Layout.fillHeight: true
    }
}
