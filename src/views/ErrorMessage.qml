import QtQuick

import QtQuick.Controls
import StyleSheet

// error message dialog
Dialog {
    id: errorMessageDialog

    property var errorList: []

    function addError(err_str) {
        errorList.push(err_str)
        errRepeater.model=errorList
    }

    function show() {
        if (!opened && errorList.length>0) {
            open()
        }
        if (width > maxWidth){
            width = maxWidth
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
    readonly property int maxWidth: root_window.width * 0.5 - leftMargin - rightMargin

    property string text: ""

    contentItem: Column {
        Repeater {
            id: errRepeater
            model: []

            delegate:Text {
                //id: message_text
                color: StyleSheet.font_color_extra
                required property string modelData


                text: modelData
                font.family: "Noto Sans"
                wrapMode: Text.WordWrap
                maximumLineCount: 10
                Component.onCompleted: {
                    console.warn("modeldata:"+modelData);
                }
            }
        }
    }

    onClosed: {
      errorList=[];
      errRepeater.model=errorList
    }
}
