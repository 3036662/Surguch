import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt.labs.platform
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

    spacing: 5

    Row {
        Layout.minimumWidth: 600
        Layout.fillWidth: true
        TopBarButton {
            icon.source: "qrc:/icons/file_plus.svg"
            text: qsTr("Open")
            onClicked: fileDialog.open()
        }
        TopBarButton {
            icon.source: "qrc:/icons/file_simple.svg"
            text: qsTr("Show in folder")
        }
        TopBarButton {
            icon.source: "qrc:/icons/folder_plus.svg"
            text: qsTr("Save as ...")
        }
        TopBarButton {
            icon.source: "qrc:/icons/printer_sm.svg"
            text: qsTr("Print")
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
            Image {
                Layout.alignment: Qt.AlignVCenter
                id: wrench_icon
                source: "qrc:/icons/wrench.svg"
                height: parent.height
                fillMode: Image.PreserveAspectFit
            }
            Rectangle {
                width: 5
                height: parent.height
                color: "transparent"
            }

            ComboBox {
                Layout.alignment: Qt.AlignVCenter
                id: profileComboBox
                model: profilesModel
                textRole: "title"
                valueRole: "value"
                displayText: defaultText
                property string defaultText: qsTr("Profile")
                implicitContentWidthPolicy: ComboBox.ContentItemImplicitWidth
                anchors.verticalCenter: parent.verticalCenter
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
                if (profileComboBox.currentValue === "new") {
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

    ToolButton {
        flat: true
        display: AbstractButton.IconOnly
        icon.source: "qrc:/icons/bell.svg"
        icon.width: 30
        icon.height: 30
        leftPadding: 10
        rightPadding: 10
    }

    FileDialog {
        id: fileDialog
        currentFile: ""
        fileMode: FileDialog.OpenFile
        nameFilters: ["Pdf files (*.pdf)"]
        folder: StandardPaths.writableLocation(StandardPaths.DocumentsLocation)
        onAccepted: {
            pdfListView.source = currentFile
            leftSideBar.source = currentFile
            rightSideBar.showState = RightSideBar.ShowState.Invisible
        }
    }

    Keys.onPressed: event => {
                        if (event.key === Qt.Key_Escape
                            && pdfListView.signMode) {
                            pdfListView.signMode = false
                            signModeButton.down = false
                        }
                    }
}
