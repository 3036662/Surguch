import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt.labs.platform
import QtCore
import StyleSheet

import "info_panel_components" as InfoPanelComponents

Flickable {
    id: root

    property var cert_data_raw
    property var stamps_data_raw
    property string profile_data
    property var profile_json
    property var cert_array
    property var cert_combo_model
    property var stamps_array
    property var stamps_combo_model
    property var profiles_model
    property int profile_id: -1

    signal profileSaved
    signal closeClicked

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
                const stamp_type_indx = selectStampTypeCombo.find(
                                          profile_json.stamp_type)
                selectStampTypeCombo.currentIndex = stamp_type_indx
                selectStampTypeCombo.item_selected = true
                if (stamp_type_indx !== -1) {
                    selectStampTypeCombo.displayText
                            = selectStampTypeCombo.model[stamp_type_indx].title
                }
                logoPath.text = profile_json.logo_path
                tspUrlEdit.text = profile_json.tsp_url
                const sign_type_indx = profile_json.create_attached
                selectFileSignType.currentIndex = sign_type_indx
                selectFileSignType.item_selected = true
                if (sign_type_indx !== -1) {
                    selectFileSignType.displayText = selectFileSignType.model[sign_type_indx].title
                }
                const encode_type_indx = profile_json.create_base_64_encoded
                selectFileEncodingType.currentIndex = encode_type_indx
                selectFileEncodingType.item_selected = true
                if (encode_type_indx !== -1) {
                    selectFileEncodingType.displayText
                            = selectFileEncodingType.model[encode_type_indx].title
                }
                const sig_ext_indx = selectFileExtension.indexOfValue(
                                       profile_json.sig_ext)
                selectFileExtension.currentIndex = sig_ext_indx
                selectFileExtension.item_selected = true
                if (sig_ext_indx !== -1) {
                    selectFileExtension.displayText = selectFileExtension.model[sig_ext_indx].title
                }
                const archive_type_indx = profile_json.pack_to_zip + profile_json.pack_separate_zips
                selectArchive.currentIndex = archive_type_indx
                selectArchive.item_selected = true
                if (archive_type_indx !== -1) {
                    selectArchive.displayText = selectArchive.model[archive_type_indx].title
                }
                root.contentY = 10
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
            selectCadesFormatCombo.currentIndex = 2
            selectCadesFormatCombo.displayText = selectCadesFormatCombo.model[2].title
            selectCadesFormatCombo.item_selected = true
            selectStampTypeCombo.currentIndex = 0
            selectStampTypeCombo.displayText = selectStampTypeCombo.model[0].title
            selectStampTypeCombo.item_selected = true
            logoPath.text = ""
            tspUrlEdit.text = ""
            selectFileSignType.currentIndex = 0
            selectFileSignType.item_selected = true
            selectFileSignType.displayText = selectFileSignType.model[0].title
            selectFileEncodingType.currentIndex = 0
            selectFileEncodingType.item_selected = true
            selectFileEncodingType.displayText = selectFileEncodingType.model[0].title
            selectFileExtension.currentIndex = 0
            selectFileExtension.item_selected = true
            selectFileExtension.displayText = selectFileExtension.model[0].title
            selectArchive.currentIndex = 0
            selectArchive.item_selected = true
            selectArchive.displayText = selectArchive.model[0].title
        }
    }

    width: parent.width
    height: parent.height
    contentHeight: profileColumn.height + 30
    leftMargin: 10
    rightMargin: 10
    topMargin: 10

    InfoPanelComponents.RSBCloseButton {
        onClicked: {
            closeClicked()
        }
    }

    Column {
        id: profileColumn
        width: parent.width

        Text {
            text: qsTr("Profile settings")
            font.weight: Font.DemiBold
            topPadding: 10
            bottomPadding: 10
            font.family: "Noto Sans"
            color: StyleSheet.font_color_extra
        }

        InfoPanelComponents.TextPair {
            visible: false
            id: profileIdTextPair
            keyText: qsTr("Profile id")
        }

        // profile name
        Text {
            text: qsTr("Profile name")
            bottomPadding: 5
            font.family: "Noto Sans"
            color: StyleSheet.font_color_extra
        }

        InfoPanelComponents.RSBTextArea {
            id: profileName
            placeholderText: qsTr("Enter profile name")
            color: StyleSheet.font_color_extra

            onTextChanged: {
                const cursorPos = cursorPosition
                const cleanedText = profileName.text.replace(/\s/g, '')
                if (cleanedText !== text) {
                    text = cleanedText
                    cursorPosition = Math.min(cursorPos - 1, text.length)
                }
            }
        }

        // certificate choice
        Text {
            text: qsTr("Certificate")
            topPadding: 10
            bottomPadding: 10
            font.family: "Noto Sans"
            color: StyleSheet.font_color_extra
        }

        InfoPanelComponents.RSBComboSelect {
            id: selectCertificateCombo

            property string displayTextDefault: qsTr("Select the certificate")

            model: root.cert_combo_model
            textRole: "title"
            valueRole: "serial"
            displayText: displayTextDefault

            property real shake: 0

            x: Math.sin(shake * Math.PI * 10) * 6

            NumberAnimation {
                id: shakeAnimation
                target: selectCertificateCombo
                property: "shake"
                from: 0
                to: 1
                duration: 200
            }
        }

        InfoPanelComponents.RightSBHorizontalDelimiter {
            width: parent.width
            topPadding: 7
            bottomPadding: 7
        }

        // use by default switch
        InfoPanelComponents.RSBSwitch {
            id: useAsDefaultProfileSwitch
            topPadding: 5
            bottomPadding: 5
            text: qsTr("Use this profile by default")
        }

        InfoPanelComponents.RightSBHorizontalDelimiter {
            width: parent.width
            topPadding: 10
            bottomPadding: 10
        }

        Text {
            text: qsTr("Signature")
            font.weight: Font.DemiBold
            topPadding: 10
            bottomPadding: 10
            font.family: "Noto Sans"
            color: StyleSheet.font_color_extra
        }

        // Cades format
        Text {
            topPadding: 5
            text: qsTr("Cades type")
            bottomPadding: 5
            font.family: "Noto Sans"
            color: StyleSheet.font_color_extra
        }

        InfoPanelComponents.RSBComboSelect {
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
            property string displayTextDefault: qsTr("Select Cades format")
            currentIndex: 2
            item_selected: true
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
                font.family: "Noto Sans"
                color: StyleSheet.font_color_extra
            }

            InfoPanelComponents.RSBTextArea {
                id: tspUrlEdit
                placeholderText: qsTr("Enter TSP service url")
                color: StyleSheet.font_color_extra
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

        InfoPanelComponents.RightSBHorizontalDelimiter {
            width: parent.width
            topPadding: 10
            bottomPadding: 10
        }

        Text {
            text: qsTr("Stamp")
            font.weight: Font.DemiBold
            topPadding: 10
            bottomPadding: 10
            font.family: "Noto Sans"
            color: StyleSheet.font_color_extra
        }

        // stamp settings
        RowLayout {
            width: parent.width

            Text {
                topPadding: 10
                text: qsTr("Stamp appearance")
                bottomPadding: 5
                font.family: "Noto Sans"
                color: StyleSheet.font_color_extra
            }

            Rectangle {
                Layout.fillWidth: true
            }

            ToolButton {
                id: editButton
                flat: true
                display: AbstractButton.IconOnly
                icon.width: 15
                icon.height: 15
                leftPadding: 5
                rightPadding: 5
                topPadding: 5
                bottomPadding: 5
                font.family: "Noto Sans"
                icon.source: StyleSheet.wrench_icon
                enabled: selectStampTypeCombo.currentText !== "ГОСТ"

                onClicked: {
                    stampEditor.stamp_json = selectStampTypeCombo.currentValue
                    stampEditor.profiles_model = profiles_model
                    let data = {
                        "CADES_format": selectCadesFormatCombo.currentValue,
                        "cert_serial": selectCertificateCombo.currentValue,
                        "logo_path": logoPath.text,
                        "tsp_url": ""
                    }
                    //console.warn(JSON.stringify(data))
                    stampEditor.profile_data = JSON.stringify(data)
                    stampEditor.updateStampForm()
                    stampEditor.editState = true
                    stampEditor.visible = true
                }
            }
        }

        RowLayout {
            width: parent.width

            MouseArea {
                Layout.fillWidth: true
                height: selectStampTypeCombo.height
                enabled: !selectStampTypeCombo.enabled
                onClicked: {
                    root.contentY = 0
                    shakeAnimation.start()
                }

                InfoPanelComponents.RSBComboSelect {
                    id: selectStampTypeCombo
                    Layout.fillWidth: true
                    model: root.stamps_combo_model
                    textRole: "title"
                    valueRole: "value"
                    enabled: selectCertificateCombo.item_selected
                             && selectCadesFormatCombo.item_selected
                    property string displayTextDefault: qsTr(
                                                            "Select stamp type")
                    currentIndex: 0
                    item_selected: true

                    onActivated: {
                        if (currentValue === "new") {
                            stampEditor.profiles_model = profiles_model
                            let data = {
                                "CADES_format": selectCadesFormatCombo.currentValue,
                                "cert_serial": selectCertificateCombo.currentValue,
                                "logo_path": logoPath.text,
                                "tsp_url": ""
                            }
                            //console.warn(JSON.stringify(data))
                            stampEditor.profile_data = JSON.stringify(data)
                            stampEditor.stamp_json = null
                            stampEditor.updateStampForm()
                            stampEditor.visible = true
                        }
                    }
                }
            }
        }

        Connections {
            target: profilesModel

            // when model has successfully saved the stamp
            function onStampsSaved(val) {
                // update stamp combobox
                rightSideBar.edit_profile.stamps_data_raw = profiles_model.getUserStampsJSON()
                // select saved stamp in the header combo
                const indx = selectStampTypeCombo.find(val)
                selectStampTypeCombo.displayText = selectStampTypeCombo.textAt(
                            indx)
                selectStampTypeCombo.currentIndex = indx
            }

            function onStampDeleted(title) {
                if (title !== "") {
                    // update stamp combobox
                    rightSideBar.edit_profile.stamps_data_raw = profiles_model.getUserStampsJSON()
                    selectStampTypeCombo.currentIndex = 0
                    selectStampTypeCombo.displayText = selectStampTypeCombo.defaultText
                }
            }
        }

        RowLayout {
            width: parent.width

            // select a logo
            Text {
                topPadding: 10
                text: qsTr("Company logo")
                bottomPadding: 5
                font.family: "Noto Sans"
                color: StyleSheet.font_color_extra
            }
            Rectangle {
                Layout.fillWidth: true
            }

            ToolButton {
                id: deleteLogo
                flat: true
                display: AbstractButton.IconOnly
                icon.width: 15
                icon.height: 15
                leftPadding: 5
                rightPadding: 5
                topPadding: 5
                bottomPadding: 5
                icon.source: StyleSheet.trash_icon

                onClicked: {
                    logoPath.text = ""
                }
            }
        }

        InfoPanelComponents.RSBTextArea {
            id: logoPath
            placeholderText: qsTr("Select a logo")
            color: StyleSheet.font_color_extra

            MouseArea {
                anchors.fill: parent

                onClicked: {
                    imgFileDialog.open()
                }
            }
        }

        // files signing settings
        InfoPanelComponents.RightSBHorizontalDelimiter {
            width: parent.width
            topPadding: 10
            bottomPadding: 10
        }

        Text {
            text: qsTr("File signing")
            font.weight: Font.DemiBold
            topPadding: 10
            bottomPadding: 10
            font.family: "Noto Sans"
            color: StyleSheet.font_color_extra
        }

        Text {
            topPadding: 5
            text: qsTr("Sign type")
            bottomPadding: 5
            font.family: "Noto Sans"
            color: StyleSheet.font_color_extra
        }

        InfoPanelComponents.RSBComboSelect {
            id: selectFileSignType
            model: [{
                    "title": qsTr("Detached")
                }, {
                    "title": qsTr("Attached")
                }]
            textRole: "title"
            valueRole: "title"
            currentIndex: 0
            item_selected: true
            property string displayTextDefault: qsTr("Select sign format")
        }

        Text {
            topPadding: 5
            text: qsTr("Certificate file encoding")
            bottomPadding: 5
            font.family: "Noto Sans"
            color: StyleSheet.font_color_extra
        }

        InfoPanelComponents.RSBComboSelect {
            id: selectFileEncodingType
            model: [{
                    "title": "DER"
                }, {
                    "title": "PEM"
                }]
            textRole: "title"
            valueRole: "title"
            currentIndex: 0
            item_selected: true
            property string displayTextDefault: qsTr("Select encoding type")
        }

        Text {
            topPadding: 5
            text: qsTr("Signature file extension")
            bottomPadding: 5
            font.family: "Noto Sans"
            color: StyleSheet.font_color_extra
        }

        InfoPanelComponents.RSBComboSelect {
            id: selectFileExtension
            model: [{
                    "title": ".sig"
                }, {
                    "title": ".sign"
                }, {
                    "title": ".sgn"
                }, {
                    "title": ".p7s"
                }, {
                    "title": ".bin"
                }]
            textRole: "title"
            valueRole: "title"
            currentIndex: 0
            item_selected: true
            property string displayTextDefault: qsTr("Select signature file extension")
        }

        Text {
            topPadding: 5
            text: qsTr("Create an archive after signing")
            bottomPadding: 5
            font.family: "Noto Sans"
            color: StyleSheet.font_color_extra
        }

        InfoPanelComponents.RSBComboSelect {
            id: selectArchive
            model: [{
                    "title": qsTr("Don't use")
                }, {
                    "title": qsTr("Common ZIP-file")
                }, {
                    "title": qsTr("Separate ZIP-file")
                }]
            textRole: "title"
            valueRole: "title"
            currentIndex: 1
            item_selected: true
            property string displayTextDefault: qsTr("Select archive after signing")
        }

        // Save button
        Item {
            width: parent.width
            height: 50

            Button {
                id: saveButton
                display: AbstractButton.TextBesideIcon
                icon.source: StyleSheet.save_icon
                icon.width: 20
                icon.height: 20
                width: logoPath.width
                //width: text.length
                //     < deleteProfileButton.text.length ? deleteProfileButton.width : 150
                text: qsTr("Save profile")
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottom: parent.bottom
                font.family: "Noto Sans"

                onClicked: {
                    if (profile_id < 0 && !profiles_model.uniqueName(
                                profileName.text)) {
                        profileName.forceActiveFocus()
                        root.contentY = 10
                        errorMessageDialog.text = qsTr(
                                    "Profile with this name already exists")
                        errorMessageDialog.open()
                        return
                    }
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
                    saveButton.enabled = false
                    profile_json = {}
                    profile_json["id"] = profile_id
                    profile_json["title"] = profileName.text
                    profile_json["use_as_default"] = useAsDefaultProfileSwitch.checked
                    profile_json["cert_serial"] = selectCertificateCombo.currentValue
                    profile_json["CADES_format"] = selectCadesFormatCombo.currentValue
                    profile_json["stamp_type"] = selectStampTypeCombo.currentText
                    profile_json["logo_path"] = logoPath.text
                    profile_json["tsp_url"] = tspUrlEdit.text
                    profile_json["create_attached"] = selectFileSignType.currentIndex
                    profile_json["create_base_64_encoded"] = selectFileEncodingType.currentIndex
                    profile_json["sig_ext"] = selectFileExtension.currentValue
                    profile_json["pack_to_zip"] = selectArchive.currentIndex > 0 ? 1 : 0
                    profile_json["pack_separate_zips"] = selectArchive.currentIndex === 2 ? 1 : 0

                    const new_profile_data = JSON.stringify(profile_json)
                    if (profiles_model.saveProfile(new_profile_data)) {
                        root.profileSaved()
                    }
                }
            }
        } // save profile end

        // delete profile
        Item {
            width: parent.width

            height: 50
            Button {
                id: deleteProfileButton
                display: AbstractButton.TextBesideIcon
                icon.source: StyleSheet.trash_icon
                icon.width: 20
                icon.height: 20
                text: qsTr("Delete profile")
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottom: parent.bottom
                width: logoPath.width
                font.family: "Noto Sans"

                onClicked: {
                    deleteProfileButton.enabled = false
                    if (profiles_model.deleteProfile(root.profile_id)) {
                        rightSideBar.showState = RightSideBar.ShowState.Invisible
                    }
                }
            }
        } // delete profile end
    }

    FileDialog {
        id: imgFileDialog
        fileMode: FileDialog.OpenFile
        nameFilters: ["Image files (*.png *.jpg *.jpeg *.bmp)"]
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
                                                      res.tooltip = qsTr(
                                                          "Issuer: ") + item.issuer_common_name
                                                      return res
                                                  })
            } catch (e) {
                console.error("Error " + e.message)
            }
        }
    }

    onStamps_data_rawChanged: {
        if (stamps_data_raw) {
            try {
                stamps_array = JSON.parse(stamps_data_raw)
                //console.warn(stamps_data_raw)
                stamps_combo_model = stamps_array.map(item => {
                                                          let res = {}
                                                          res.title = item.title
                                                          res.value = (item.id === 0) ? "new" : item
                                                          return res
                                                      })
            } catch (e) {
                console.error("Error " + e.message)
            }
        }
    }

    onVisibleChanged: {
        if (visible) {
            saveButton.enabled = true
            deleteProfileButton.enabled = true
        }
    }
}
