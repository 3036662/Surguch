import QtQuick

import QtQuick.Controls
import StyleSheet

// error message dialog
Dialog {
    id: errorMessageDialog

    function addError(err_str) {
        if (text != "") {
            errorMessageDialog.text += "\n"
        }
        text += err_str
    }

    function show() {
        if (!opened && text != "") {
            open()
        }
    }

    modal: true
    title: qsTr("Error")
    standardButtons: Dialog.Ok
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2
    topPadding: StyleSheet.defaultPaddingV
    bottomPadding: StyleSheet.defaultPaddingV
    leftPadding: StyleSheet.defaultPaddingH
    rightPadding: StyleSheet.defaultPaddingH
    topMargin: StyleSheet.defaultMarginV
    bottomMargin: StyleSheet.defaultMarginV
    leftMargin: StyleSheet.defaultMarginH
    rightMargin: StyleSheet.defaultMarginH
    readonly property int maxWidth: root_window.width*0.5 - leftMargin - rightMargin

    Component.onCompleted: {
        if (width > maxWidth)
            width = maxWidth
    }

    property string text: ""

    contentItem: Text {
        id: message_text
        color: StyleSheet.font_color_extra

        text: errorMessageDialog.text
        font.family: "Noto Sans"
        wrapMode: Text.WordWrap
        maximumLineCount: 10
    }

    onClosed: {
        text = ""
    }
}
