import QtQuick
import QtQuick.Controls
import StyleSheet

// Info dialog in center of window
Dialog {
    id: infoDialog
    property alias content: infoDialogContentContainer


    width: root_window.width - 200
    height: root_window.height - 100
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

    Loader {
        id: infoDialogContentContainer
        width: parent.width
        height: parent.height
    }
}
