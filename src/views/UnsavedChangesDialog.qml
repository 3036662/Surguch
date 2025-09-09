import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts
import StyleSheet

// unsave changed dialog
// MessageDialog with standard buttons is not utilized because
// button size is invalid for Russian button text.
Dialog {
    id: unsavedFileDialog

    signal saveWithQuit(bool need_quit)

    width: 300
    height: unsavedFileDialogContent.height + 10
    modal: true
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

    Column {

        id: unsavedFileDialogContent

        width: parent.width
        spacing: 10
        anchors.verticalCenter: parent.verticalCenter
        topPadding: StyleSheet.defaultPaddingV
        bottomPadding: StyleSheet.defaultPaddingV
        leftPadding: StyleSheet.defaultPaddingH
        rightPadding: StyleSheet.defaultPaddingH
        anchors.topMargin: StyleSheet.defaultMarginV
        anchors.bottomMargin: StyleSheet.defaultMarginV
        anchors.leftMargin: StyleSheet.defaultMarginH
        anchors.rightMargin: StyleSheet.defaultMarginH

        Text {
            text: qsTr("Unsaved Changes")
            wrapMode: Text.Wrap
            color: StyleSheet.font_color_extra
            horizontalAlignment: Text.AlignHCenter
            anchors.horizontalCenter: parent.horizontalCenter
            font.bold: true
        }

        Text {
            text: qsTr("Do you want to save your changes?")
            wrapMode: Text.Wrap
            color: StyleSheet.font_color_extra
            horizontalAlignment: Text.AlignHCenter
            anchors.horizontalCenter: parent.horizontalCenter
        }
        RowLayout {
            spacing: 10
            anchors.horizontalCenter: parent.horizontalCenter

            Button {
                text: qsTr("Save")
                width: 100
                onClicked: {
                    unsavedFileDialog.close()
                    saveWithQuit(true)
                }
            }

            Button {
                text: qsTr("Discard")
                width: 100
                onClicked: {
                    unsavedFileDialog.close()
                    Qt.quit()
                }
            }

            Button {
                text: qsTr("Cancel")
                width: 100
                onClicked: {
                    unsavedFileDialog.close() // Just close the dialog
                }
            }
        }
    }
}
