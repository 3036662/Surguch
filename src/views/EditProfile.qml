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

    RSBCloseButton {}

    Column {
        width: parent.width

        Text {
            text: qsTr("Profile settings")
            font.weight: Font.DemiBold
            topPadding: 10
            bottomPadding: 10
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
            topPadding: 5
            bottomPadding: 5
            id: useAsDefaultProfileSwitch
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
            displayText: qsTr("Select the certificate")
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
            displayText: qsTr("Select Cades format")
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
            displayText: qsTr("Select stamp type")
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

            onTextChanged: {

                //copy an image
            }

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

                onTextChanged: {
                    try {
                        new URL(tspUrlEdit.text)
                        tspUrlEdit.color = "green"
                    } catch (err) {
                        tspUrlEdit.color = "red"
                    }
                }
            }
        }

        // Save button
        Item {
            width: parent.width
            height: 50

            Button {
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
                    }

                    // no check for logo
                    if (tspUrlWrapper.visible && tspUrlEdit.color === "red") {
                        tspUrlEdit.forceActiveFocus()
                        root.contentY = 300
                    }
                }
            }
        }
    }

    FileDialog {
        id: imgFileDialog
        fileMode: FileDialog.OpenFile
        nameFilters: ["Img files (*.png)"]
        options: FileDialog.ReadOnly
        folder: StandardPaths.writableLocation(StandardPaths.DocumentsLocation)
        onAccepted: {
            logoPath.text = currentFile
        }
    }

    onProfile_dataChanged: {
        if (profile_data) {
            try {
                profile_json = JSON.parse(profile_data)
            } catch (e) {
                console.error("Error parsing JSON" + e.message)
            }
        }
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
                                                      res.value = item.serial
                                                      return res
                                                  })
            } catch (e) {
                console.error("Error " + e.message)
            }
        }
    }
}
