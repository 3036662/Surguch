import QtQuick
import alt.pdfcsp.signatureCreator

Item {
    id: root

    // estimate the resulting stamp size
    function resizeAim(location_data) {
        try {
            if (typeof (location_data) == "undefined") {
                return
            }
            let params = gatherParams(location_data)
            sigCreator.estimateStampResizeFactor(params)
        } catch (e) {
            console.warn("resizeAim" + e)
        }
    }

    function saveLastAimSize(location) {        
        private_data.lastLocation = location
    }


    // common function to gather parameters used in resizeAim,signDoc and StampPreview::createPreview

    /* When called from the resizeAim function, it receives only location_data.
     *
     * When called from the signDoc function, it receives only location_data and a path to the file to sign.
     *
     * When called from the createPreview function of the StampPreview object,
     * it receives no location data and no path, only the custom_profile_data and custom_stamp_json.
     * In this case the location data will be copied from private_data.lastLocation.
     */
    function gatherParams(location_data, path, custom_profile_data, custom_stamp_json) {
        let curr_profile = {}
        let cert_array = {}
        if (!custom_profile_data) {
            curr_profile = JSON.parse(header.getCurrentProfileValue())
        } else {
            curr_profile = JSON.parse(custom_profile_data)
        }
        cert_array = JSON.parse(profilesModel.getUserCertsJSON())
        if (!location_data) {
            location_data = private_data.lastLocation
        }
        let cert_index = cert_array.findIndex(cert => {
                                                  return curr_profile.cert_serial === cert.serial
                                              })
        if (cert_index === -1) {
            errorMessageDialog.text = qsTr(
                        "Certificate not found, looks like it was deleted.﻿")
            errorMessageDialog.open()
            throw new Error('Certificate data not found')
        }
        let stamps_json
        let user_stamp
        if (!custom_stamp_json) {
            stamps_json = JSON.parse(profilesModel.getUserStampsJSON())
            user_stamp = stamps_json.find(stamp => {
                                              return curr_profile.stamp_type === stamp.title
                                          })
        } else {
            // TODO(Oleg) why R G B?
            user_stamp = custom_stamp_json
            user_stamp.R = custom_stamp_json.border_color_red
            user_stamp.G = custom_stamp_json.border_color_green
            user_stamp.B = custom_stamp_json.border_color_blue
            user_stamp.transparent=custom_stamp_json.bg_transparent
        }

        // gather all information needed to create a signature visual representation
        let params = {
            "page_index": location_data.page_index,
            "page_width": location_data.page_width,
            "page_height": location_data.page_height,
            "stamp_x": location_data.stamp_x,
            "stamp_y": location_data.stamp_y,
            "stamp_width": location_data.stamp_width,
            "stamp_height": location_data.stamp_height,
            "logo_path": curr_profile.logo_path,
            "config_path": profilesModel.getConfigPath(),
            "cert_serial": curr_profile.cert_serial,
            "cert_serial_prefix": qsTr("Certificate: "),
            "cert_subject": cert_array[cert_index].subject_common_name,
            "cert_subject_prefix": qsTr("Subject: "),
            "cert_time_validity": qsTr("Vaildity: ")
                                  + cert_array[cert_index].not_before_readable + qsTr(
                " till ") + cert_array[cert_index].not_after_readable,
            "stamp_title": qsTr("THE DOCUMENT IS SIGNED WITH AN ELECTRONIC SIGNATURE"),
            "stamp_type": curr_profile.stamp_type,
            "text_color_red": user_stamp.R,
            "text_color_green": user_stamp.G,
            "text_color_blue": user_stamp.B,
            "border_color_red": user_stamp.R,
            "border_color_green": user_stamp.G,
            "border_color_blue": user_stamp.B,
            "border_width": user_stamp.border_width,
            "border_radius": user_stamp.border_radius,
            "bg_transparent": user_stamp.transparent,
            "bg_opacity": 1,
            "cades_type": curr_profile.CADES_format,
            "tsp_url": curr_profile.tsp_url,
            "file_to_sign_path": path
        }
        //console.warn(JSON.stringify(params))
        return params
    }

    Component.onCompleted: {
        // sign creation finished
        sigCreator.signCompleted.connect(sigCreator.handleSigResult)
        // stamp location selected
        pdfListView.stampLocationSelected.connect(sigCreator.signDoc)
        // stamp size estimated
        sigCreator.stampSizeEstimated.connect(
                    pdfListView.updateStampResizeFactor)
    }

    Item {
        id: private_data
        property var lastLocation
    }

    SignatureCreator {
        id: sigCreator

        // sign the document
        function signDoc(location_data, path) {
            try {
                if (typeof (location_data) == "undefined") {
                    return
                }
                let params = root.gatherParams(location_data, path)
                console.warn("[debug resize]" + JSON.stringify(params))
                sigCreator.createSignature(params)
            } catch (e) {
                console.warn("signDoc" + e)
            }
        }

        // handle the result of signDoc function
        function handleSigResult(result) {
            console.warn(result.status)
            if (!result.status) {
                if (result.err_string === "CERT_EXPIRED") {
                    errorMessageDialog.text = qsTr(
                                "Your certificate is expired.")
                } else if (result.err_string === "MAYBE_TSP_URL_INVALID") {
                    errorMessageDialog.text = qsTr(
                                "Common error. It looks like the TSP URL is not valid.")
                } else if (result.err_string === "CERT_CHAINING_ERR") {
                    errorMessageDialog.text = qsTr(
                                "Certificate chain error happened, it looks like one of root certificates is missing or is not in trusted list.")
                } else if (result.err_string === "TIMEOUT") {
                    errorMessageDialog.text = qsTr("Error.Timeout exceeded.")
                } else {
                    errorMessageDialog.text = qsTr("Common error")
                }
                errorMessageDialog.open()
            } // if successfully signed
            else {
                if (result.tmp_file_path !== undefined) {
                    // open with openTmpFile, to be deleted later
                    pdfListView.openTmpFile(result.tmp_file_path)
                    leftSideBar.source = result.tmp_file_path
                    rightSideBar.showState = RightSideBar.ShowState.Invisible
                }
            }
            header.enableSignMode()
        }
    }
}
