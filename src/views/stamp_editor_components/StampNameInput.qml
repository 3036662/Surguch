import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import StyleSheet

ColumnLayout {

    property string labelText
    property alias text: stampNameTextArea.text
    property alias placeholderText: stampNameTextArea.placeholderText

    function forceActiveFocus() {
        stampNameTextArea.forceActiveFocus()
    }

    // stamp name label
    Text {
        text: labelText
        topPadding: 5
        bottomPadding: 5
        font.family: "Noto Sans"
        color: StyleSheet.font_color_extra
    }

    TextArea {
        id: stampNameTextArea
        Layout.fillWidth: true
        selectByMouse: true
        wrapMode: Text.WordWrap
        placeholderTextColor: "grey"
        font.family: "Noto Sans"
        color: StyleSheet.font_color_extra
        topPadding: 5
        bottomPadding: 5

        background: Rectangle {
            border.color: StyleSheet.slider_border_color
            color: StyleSheet.text_area_background
            radius: 4
        }

        onTextChanged: {
            let validInput = stampNameTextArea.text.match(/^S+$/)
            if (!validInput) {
                stampNameTextArea.text = stampNameTextArea.text.replace(/\s/g,
                                                                        '')
                stampNameTextArea.cursorPosition = stampNameTextArea.text.length
            }
            if (stampNameTextArea.text.length > 50) {
                stampNameTextArea.text = stampNameTextArea.text.slice(0, 50)
            }
        }
    }
}
