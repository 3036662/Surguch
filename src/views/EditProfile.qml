import QtQuick
import QtQuick.Controls
import Qt.labs.platform
import QtCore

Flickable {
    id: root
    width: parent.width
    height: parent.height
    contentHeight: 1000
    leftMargin: 10
    rightMargin: 10
    topMargin: 10

    property var cert_data_raw
    property string profile_data
    property var profile_json
    property var cert_array
    property var cert_combo_model
    property var profiles_model
    property int profile_id: -1

    RSBCloseButton {}

    Column {
        width: parent.width

        Text {
            text: qsTr("Profile settings")
            font.weight: Font.DemiBold
            topPadding: 10
            bottomPadding: 10
        }

        TextPair {
            id: profileIdTextPair
            keyText: qsTr("Profile id")
        }

        // profile name
        Text {
            text: qsTr("Profile name")
            bottomPadding: 5
        }

        RSBTextArea {
            id: profileName
            placeholderText: qsTr("Enter profile name")

            onTextChanged: {
                let validInput = profileName.text.match(/^[a-zA-Z0-9]*$/)
                if (!validInput) {
                    profileName.text = profileName.text.replace(
                                /[^a-zA-Z0-9s]/g, '')
                    profileName.cursorPosition = profileName.text.length
                }
            }
        }

        RightSBHorizontalDelimiter {
            width: parent.width
        }

        // use by default switch
        RSBSwitch {
            id: useAsDefaultProfileSwitch
            topPadding: 5
            bottomPadding: 5
            text: qsTr("Use this profile by default")
        }

        RightSBHorizontalDelimiter {
            width: parent.width
        }

        Text {
            text: qsTr("Mandatory settings")
            font.weight: Font.DemiBold
            topPadding: 10
            bottomPadding: 10
        }

        // certificate choice
        Text {
            text: qsTr("Certificate")
            bottomPadding: 5
        }

        RSBComboSelect {
            id: selectCertificateCombo
            model: root.cert_combo_model
            textRole: "title"
            valueRole: "serial"
            displayText: displayTextDefault
            property string displayTextDefault: qsTr("Select the certificate")
        }

        // Cades format
        Text {
            topPadding: 5
            text: qsTr("Cades type")
            bottomPadding: 5
        }

        RSBComboSelect {
            id: selectCadesFormatCombo
            model: [{
                    "title": "CADES_BES"
                }, {
                    "title": "CADES_T"
                }, {
                    "title": "CADES_XLT1"
                }]
            textRole: "title"
            valueRole: "title"
            displayText: displayTextDefault
            property string displayTextDefault: qsTr("Select Cades format")
        }

        // stamp settings
        Text {
            topPadding: 10
            text: qsTr("Stamp appearance")
            bottomPadding: 5
        }

        RSBComboSelect {
            id: selectStampTypeCombo
            model: [{
                    "title": "ГОСТ"
                }]
            textRole: "title"
            valueRole: "title"
            displayText: displayTextDefault
            property string displayTextDefault: qsTr("Select stamp type")
        }

        // select a logo
        Text {
            topPadding: 10
            text: qsTr("Company logo")
            bottomPadding: 5
        }

        RSBTextArea {
            id: logoPath
            placeholderText: qsTr("Select a logo")

            MouseArea {
                anchors.fill: parent

                onClicked: {
                    imgFileDialog.open()
                }
            }
        }

        // tsp url
        Column {
            id: tspUrlWrapper
            width: parent.width
            visible: selectCadesFormatCombo.item_selected
                     && (selectCadesFormatCombo.currentValue === "CADES_T"
                         || selectCadesFormatCombo.currentValue === "CADES_XLT1")
            Text {
                topPadding: 10
                text: qsTr("TSP server URL")
                bottomPadding: 5
            }

            RSBTextArea {
                id: tspUrlEdit
                placeholderText: qsTr("Enter TSP service url")
                inputMethodHints: Qt.ImhUrlCharactersOnly
                property bool valid_url: false

                onTextChanged: {
                    try {
                        new URL(tspUrlEdit.text)
                        tspUrlEdit.color = "green"
                        tspUrlEdit.valid_url = true
                    } catch (err) {
                        tspUrlEdit.color = "red"
                        tspUrlEdit.valid_url = false
                    }
                }
            }
        }

        // Save button
        Item {
            width: parent.width
            height: 50

            Button {
                id: saveButton
                width: deleteProfileButton.width
                text: qsTr("Save profile")
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottom: parent.bottom

                onClicked: {
                    if (profileName.text === "") {
                        profileName.forceActiveFocus()
                        root.contentY = 10
                        return
                    }
                    if (!selectCertificateCombo.item_selected) {
                        selectCertificateCombo.forceActiveFocus()
                        root.contentY = 20
                        return
                    }
                    if (!selectCadesFormatCombo.item_selected) {
                        selectCadesFormatCombo.forceActiveFocus()
                        root.contentY = 50
                        return
                    }

                    if (!selectStampTypeCombo.item_selected) {
                        selectStampTypeCombo.forceActiveFocus()
                        root.contentY = 200
                        return
                    }

                    if (tspUrlWrapper.visible && !tspUrlEdit.valid_url) {
                        tspUrlEdit.forceActiveFocus()
                        root.contentY = 300
                        return
                    }
                    saveButton.enabled=false;
                    profile_json = {}
                    profile_json["id"] = profile_id
                    profile_json["title"] = profileName.text
                    profile_json["use_as_default"] = useAsDefaultProfileSwitch.checked
                    profile_json["cert_serial"] = selectCertificateCombo.currentValue
                    profile_json["CADES_format"] = selectCadesFormatCombo.currentValue
                    profile_json["stamp_type"] = selectStampTypeCombo.currentValue
                    profile_json["logo_path"] = logoPath.text
                    profile_json["tsp_url"] = tspUrlEdit.text
                    const new_profile_data = JSON.stringify(profile_json)
                    console.warn(profiles_model.saveProfile(new_profile_data))
                }
            }
        } // save profile end

        // delete profile
        Item {
            width: parent.width
            height: 50
            Button {
                id: deleteProfileButton
                text: qsTr("Delete profile")
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottom: parent.bottom

                onClicked: {
                      deleteProfileButton.enabled=false;
                      if (profiles_model.deleteProfile(root.profile_id)){
                            rightSideBar.showState = RightSideBar.ShowState.Invisible
                      }
                }
            }
        } // delete profile end
    }

    FileDialog {
        id: imgFileDialog
        fileMode: FileDialog.OpenFile
        nameFilters: ["Png files (*.png)","jpg files (*.jpg *.jpeg)"]
        options: FileDialog.ReadOnly
        folder: StandardPaths.writableLocation(StandardPaths.DocumentsLocation)
        onAccepted: {
            logoPath.text = currentFile
        }
    }

    onProfile_dataChanged: {
        updateProfileForm()
    }

    onCert_data_rawChanged: {
        if (cert_data_raw) {
            try {
                cert_array = JSON.parse(cert_data_raw)
                //console.warn(JSON.stringify(cert_array));
                cert_combo_model = cert_array.map(item => {
                                                      let res = {}
                                                      res.title = item.subject_common_name
                                                      + " " + item.serial
                                                      res.serial = item.serial
                                                      return res
                                                  })
            } catch (e) {
                console.error("Error " + e.message)
            }
        }
    }

    onVisibleChanged: {
        if (visible){
            saveButton.enabled=true;
            deleteProfileButton.enabled=true;
        }
    }

    // fill the form from profile_data JSON string
    function updateProfileForm() {
        if (profile_data) {
            try {
                profile_json = JSON.parse(profile_data)
                profile_id = profile_json.id
                profileIdTextPair.value = profile_json.id
                profileName.text = profile_json.title
                useAsDefaultProfileSwitch.checked = profile_json.use_as_default
                const cert_indx = selectCertificateCombo.indexOfValue(
                                    profile_json.cert_serial)
                selectCertificateCombo.currentIndex = cert_indx
                selectCertificateCombo.item_selected = true
                if (cert_indx !== -1) {
                    selectCertificateCombo.displayText = cert_combo_model[cert_indx].title
                }
                const cades_format_indx = selectCadesFormatCombo.indexOfValue(
                                            profile_json.CADES_format)
                selectCadesFormatCombo.currentIndex = cades_format_indx
                selectCadesFormatCombo.item_selected = true
                if (cades_format_indx !== -1) {
                    selectCadesFormatCombo.displayText
                            = selectCadesFormatCombo.model[cades_format_indx].title
                }
                const stamp_type_indx = selectStampTypeCombo.indexOfValue(
                                          profile_json.stamp_type)
                selectStampTypeCombo.currentIndex = stamp_type_indx
                selectStampTypeCombo.item_selected = true
                if (stamp_type_indx !== -1) {
                    selectStampTypeCombo.displayText
                            = selectStampTypeCombo.model[stamp_type_indx].title
                }
                logoPath.text = profile_json.logo_path
                tspUrlEdit.text = profile_json.tsp_url
            } catch (e) {
                console.error("Error parsing JSON" + e.message)
            }   
        } else {
            // fill with empty/default values
            profile_id = -1
            profileIdTextPair.value = profile_id
            profileName.text = ""
            useAsDefaultProfileSwitch.checked = false
            selectCertificateCombo.currentIndex = 0
            selectCertificateCombo.item_selected = false
            selectCertificateCombo.displayText = selectCertificateCombo.displayTextDefault
            selectCadesFormatCombo.currentIndex = 0
            selectCadesFormatCombo.item_selected = false
            selectCadesFormatCombo.displayText = selectCadesFormatCombo.displayTextDefault
            selectStampTypeCombo.currentIndex = 0
            selectStampTypeCombo.item_selected = false
            selectStampTypeCombo.displayText = selectStampTypeCombo.displayTextDefault
            logoPath.text = ""
            tspUrlEdit.text = ""
        }
    }
}
