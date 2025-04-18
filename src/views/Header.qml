import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt.labs.platform as LabsDialogs

import QtQuick.Dialogs as CommonDialods
import QtCore

RowLayout {
    id: toolbar_layout

    function getCurrentProfileValue() {
        return profileComboBox.currentValue
    }

    function disableSignMode() {
        console.warn("disable sign mode")
        signModeButton.down = false
        signModeButton.enabled = false
        pdfListView.signMode = false
    }

    function enableSignMode() {
        signModeButton.down = false
        signModeButton.enabled = true
        pdfListView.signMode = false
        pdfListView.signInProgress = false
    }

    function launchSaveFileWithQuit(quit_after_save) {
        let dlg;
        if (kdeVersion === "5") {
           dlg=labsSaveFileDialog;
        } else {
            dlg=saveFileDialog;
        }
        if (quit_after_save){
            dlg.quitAfterSave=true;
        }
        dlg.open();
    }


    spacing: 5

    Row {
        Layout.minimumWidth: 600
        Layout.fillWidth: true

        TopBarButton {
            icon.source: "qrc:/icons/file_plus.svg"
            text: qsTr("Open")
            onClicked: kdeVersion === "5" ? labsFileDialog.open(
                                                ) : fileDialog.open()
            width: 130
        }

        TopBarButton {
            icon.source: "qrc:/icons/file_simple.svg"
            text: qsTr("Show in folder")
            enabled: pdfListView.source.length > 0 && !pdfListView.sourceIsTmp
            onClicked: pdfListView.showInFolder()
        }

        TopBarButton {
            icon.source: "qrc:/icons/folder_plus.svg"
            text: qsTr("Save as ...")
            enabled: pdfListView.source.length > 0
            onClicked: kdeVersion === "5" ? labsSaveFileDialog.open(
                                                ) : saveFileDialog.open()
        }

        TopBarButton {
            icon.source: "qrc:/icons/printer_sm.svg"
            text: qsTr("Print")
            enabled: pdfListView.source.length > 0
            onClicked: printer.print(pdfListView.source, pdfListView.count,
                                     pdfListView.landscape)
        }

        Row {
            spacing: 2
            id: comboBox_row
            height: parent.height
            Rectangle {
                width: 5
                height: parent.height
                color: "transparent"
            }
            TopBarButton {
                icon.source: "qrc:/icons/wrench.svg"
                enabled: profileComboBox.currentValue !== "new"

                onClicked: {
                    //open profile info panel
                    rightSideBar.showState = RightSideBar.ShowState.ProfileInfo
                    // set the certificates for select
                    rightSideBar.edit_profile.cert_data_raw
                            = profileComboBox.model.getUserCertsJSON()
                    // set a reference to this model
                    rightSideBar.edit_profile.profiles_model = profileComboBox.model
                    rightSideBar.edit_profile.profile_data = profileComboBox.currentValue
                }
            }

            Rectangle {
                width: 5
                height: parent.height
                color: "transparent"
            }

            ComboBox {
                id: profileComboBox

                property string defaultText: qsTr("Profile")

                font.family: "Noto Sans"
                model: profilesModel
                textRole: "title"
                valueRole: "value"
                displayText: defaultText
                width: 200
                //implicitContentWidthPolicy: ComboBox.WidestText
                //implicitContentWidthPolicy: ComboBox.ContentItemImplicitWidth
                popup.y: profileComboBox.height

                anchors.verticalCenter: parent.verticalCenter
                Layout.alignment: Qt.AlignVCenter

                onActivated: {
                    profileComboBox.displayText = profileComboBox.textAt(
                                currentIndex)
                    //open profile info panel
                    rightSideBar.showState = RightSideBar.ShowState.ProfileInfo
                    // set the certificates for select
                    rightSideBar.edit_profile.cert_data_raw
                            = profileComboBox.model.getUserCertsJSON()
                    // set a reference to this model
                    rightSideBar.edit_profile.profiles_model = profileComboBox.model
                    // if create a new profile, set an empty data
                    if (currentValue === "new") {
                        rightSideBar.edit_profile.profile_data = ""
                        rightSideBar.edit_profile.profile_id = -1
                        // ele set current profile data
                    } else {
                        rightSideBar.edit_profile.profile_data = currentValue
                    }
                }

                Component.onCompleted: {
                    let def_profile = profilesModel.getDetDefaultProfileVal()
                    if (def_profile !== "") {
                        const indx = profileComboBox.indexOfValue(def_profile)
                        profileComboBox.displayText = profileComboBox.textAt(
                                    indx)
                        profileComboBox.currentIndex = indx
                    } else {
                        profileComboBox.currentIndex = -1
                    }
                }
            }

            Connections {
                target: profilesModel
                // when model has successfully saved the profile
                function onProfileSaved(val) {
                    // select saved profile in the header combo
                    const indx = profileComboBox.indexOfValue(val)
                    profileComboBox.displayText = profileComboBox.textAt(indx)
                    profileComboBox.currentIndex = indx
                    // update profile in right side bar
                    rightSideBar.edit_profile.profile_data = profileComboBox.currentValue
                    rightSideBar.edit_profile.updateProfileForm()
                    rightSideBar.showState = RightSideBar.ShowState.Invisible
                }

                function onProfileDeleted(title) {
                    if (title !== "") {
                        profileComboBox.currentIndex = 0
                        profileComboBox.displayText = profileComboBox.defaultText
                    }
                }
            }

            Rectangle {
                width: 5
                height: parent.height
                color: "transparent"
            }
        }
        TopBarButton {
            id: signModeButton
            icon.source: "qrc:/icons/pencil_line.svg"
            text: qsTr("Sign")
            icon.height: 25
            icon.width: 25
            anchors.top: parent.top
            anchors.bottom: parent.bottom            

            onClicked: {
                if (profileComboBox.currentValue === "new"
                        || profileComboBox.currentIndex === -1) {
                    profileComboBox.popup.open()
                } else {
                    pdfListView.signMode = !pdfListView.signMode
                    if (!down) {
                        pdfListView.reserRotation()
                    }
                    down = !down
                }
            }
        }
    }

    // ToolButton {
    //     flat: true
    //     display: AbstractButton.IconOnly
    //     icon.source: "qrc:/icons/bell.svg"
    //     icon.width: 30
    //     icon.height: 30
    //     leftPadding: 10
    //     rightPadding: 10
    // }
    Keys.onPressed: event => {
                        if (event.key === Qt.Key_Escape
                            && pdfListView.signMode) {
                            pdfListView.signMode = false
                            signModeButton.down = false
                            event.accepted = true
                            return
                        }
                        event.accepted = false
                    }

    CommonDialods.FileDialog {
        id: fileDialog
        fileMode: CommonDialods.FileDialog.OpenFile
        //nameFilters: ["PDF files (*.pdf)","Any file (* *.*)"];
        nameFilters: [qsTr("PDF files (*.pdf)"), qsTr("Any file (* *.*)")]
        currentFolder: StandardPaths.writableLocation(
                           StandardPaths.DocumentsLocation)
        onAccepted: {
            enableSignMode();
            // source is chosen by user, not a temporary file
            pdfListView.openFile(currentFile)
            leftSideBar.source = currentFile
            rightSideBar.showState = RightSideBar.ShowState.Invisible
        }
    }

    CommonDialods.FileDialog {
        id: saveFileDialog

        property bool quitAfterSave: false

        fileMode: CommonDialods.FileDialog.SaveFile
        defaultSuffix: "pdf"
        nameFilters: [qsTr("PDF files (*.pdf)")]
        currentFolder: StandardPaths.writableLocation(
                           StandardPaths.DocumentsLocation)

        onAccepted: {
            console.warn(currentFile)
            pdfListView.saveTo(currentFile)
            if (quitAfterSave){
                Qt.quit();
            }
        }
    }

    // KDE5 - use lab LabsDialogs
    LabsDialogs.FileDialog {
        id: labsFileDialog

        currentFile: ""
        fileMode: LabsDialogs.FileDialog.OpenFile
        nameFilters: [qsTr("PDF files (*.pdf)"), qsTr("Any file (* *.*)")]
        folder: StandardPaths.writableLocation(StandardPaths.DocumentsLocation)
        onAccepted: {
            enableSignMode();
            // source is chosen by user, not a temporary file
            pdfListView.openFile(currentFile)
            leftSideBar.source = currentFile
            rightSideBar.showState = RightSideBar.ShowState.Invisible
        }
    }
    // KDE5 - use lab LabsDialogs
    LabsDialogs.FileDialog {
        id: labsSaveFileDialog

        property bool quitAfterSave: false

        fileMode: LabsDialogs.FileDialog.SaveFile
        defaultSuffix: "pdf"
        currentFile: pdfListView.source
        nameFilters: ["PDF files (*.pdf)"]
        folder: StandardPaths.writableLocation(StandardPaths.DocumentsLocation)

        onAccepted: {
            console.warn(currentFile)
            pdfListView.saveTo(currentFile)
            if (quitAfterSave){
                Qt.quit();
            }
        }
    }
}
