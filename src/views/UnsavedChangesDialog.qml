import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts

// unsave changed dialog
// MessageDialog with standard buttons is not utilized because
// button size is invalid for Russian button text.
Dialog {
    id: undsavedFileDialog

    signal saveWithQuit(bool need_quit);

    width:300
    height:unsavedFileDialogContent.height+50
    title: qsTr("Unsaved Changes")
    modal: true
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2

    Column {
        id:unsavedFileDialogContent

        width:parent.width
        spacing: 10
        anchors.verticalCenter: parent.verticalCenter

        Text{
            text: qsTr("Do you want to save your changes?")
            wrapMode: Text.Wrap
        }
        RowLayout {
            spacing: 10
            anchors.horizontalCenter: parent.horizontalCenter

            Button {
                text: qsTr("Save")
                width: 100
                onClicked: {
                    undsavedFileDialog.close();
                    saveWithQuit(true);
                    //header.launchSaveFileWithQuit(true);
                }
            }

            Button {
                text: qsTr("Discard")
                width: 100
                onClicked: {
                    undsavedFileDialog.close();
                    Qt.quit();
                }
            }

            Button {
                text: qsTr("Cancel")
                width: 100
                onClicked: {
                    undsavedFileDialog.close(); // Just close the dialog
                }
            }
        }

    }
}
