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


function editButtonClicked(){
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

function selectStampTypeComboActivated(){
    if (selectStampTypeCombo.currentValue === "new") {
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

function stamptSaved(val){
    // update stamp combobox
    rightSideBar.edit_profile.stamps_data_raw = profiles_model.getUserStampsJSON()
    // select saved stamp in the header combo
    const indx = selectStampTypeCombo.find(val)
    selectStampTypeCombo.displayText = selectStampTypeCombo.textAt(
                indx)
    selectStampTypeCombo.currentIndex = indx
}

function stampDeleted(title){
    if (title !== "") {
        // update stamp combobox
        rightSideBar.edit_profile.stamps_data_raw = profiles_model.getUserStampsJSON()
        selectStampTypeCombo.currentIndex = 0
        selectStampTypeCombo.displayText = selectStampTypeCombo.defaultText
    }
}

function saveButtonClicked(){
    if (profile_id < 0 && !profiles_model.uniqueName(
                profileName.text)) {
        profileName.forceActiveFocus()
        root.contentY = 10
        errorMessageDialog.addError(qsTr(
                    "Profile with this name already exists"))
        errorMessageDialog.show()
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

function certDataRawChanged(){
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

function stampDataRawChanged(){
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

