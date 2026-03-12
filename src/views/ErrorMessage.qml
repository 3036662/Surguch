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
    }



    modal: true

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

    header:Label{
        topPadding: StyleSheet.defaultPaddingV
        leftPadding: StyleSheet.defaultPaddingH
        id:titleLabel
        text: qsTr("Error")
        font.bold: true
    }

    contentItem: Column {
        id: contentColumn

        signal widthCorrection;

        // the width of the widest text element
        property int maxErrWidth : 0

        // update the widest element width
        function textAdded(w){
            if (w>maxErrWidth){
                maxErrWidth=w;
                widthCorrection();
            }
            //console.warn("TEXT add , width:"+w," maxErrWidth="+maxErrWidth +" max windows size: "+errorMessageDialog.maxWidth);
        }

        Repeater {
            id: errRepeater
            model: []


            delegate:Text {                
                required property string modelData

                topPadding:10;
                color: StyleSheet.font_color_extra

                text: modelData
                font.family: "Noto Sans"
                wrapMode: Text.WordWrap
                maximumLineCount: 10

                Component.onCompleted: {
                    // register the current width, make sure it is not wider then the dialog's maxWidth
                    width=Math.min(errorMessageDialog.maxWidth-errorMessageDialog.leftPadding-errorMessageDialog.rightPadding,width)
                    contentColumn.textAdded(width);
                }

                Connections {
                    target: contentColumn

                    function onWidthCorrection(){
                        width=Math.min(errorMessageDialog.maxWidth,contentColumn.maxErrWidth)
                        contentColumn.textAdded(width);
                    }

                }
            }
        }
    }

    onClosed: {
      contentColumn.maxErrWidth=0;
      errorList=[];
      errRepeater.model=errorList
    }
}
