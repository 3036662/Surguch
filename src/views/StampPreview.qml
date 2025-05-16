import QtQuick
import alt.pdfcsp.previewRender

Item {
    id: root

    property var profile_data
    property var image_data
    property var stamp_data
    property bool processing: false
    property bool new_requested: false
    property bool window_completed: false

    function setStampData() {
        //console.warn("preview " + profile_data)
        // let curr_profile
        if (!profile_data) {
            return {}
        }

        let curr_profile = JSON.parse(profile_data)
        let cert_array = JSON.parse(profilesModel.getUserCertsJSON())
        // console.warn(JSON.stringify(rightSideBar.edit_profile.cert_array));
        let cert_index = cert_array.findIndex(cert => {
                                                  return curr_profile.cert_serial === cert.serial
                                              })
        if (cert_index === -1) {
            errorMessageDialog.text = qsTr(
                        "Certificate not found, looks like it was deleted.﻿")
            errorMessageDialog.open()
            throw new Error('Certificate data not found')
        }
        // gather all information needed to create a signature visual representation
        let params = {
            "page_index": 0,
            "page_width"//location_data.page_index,
            : 0,
            "page_height"//location_data.page_width,
            : 0,
            "stamp_x"//location_data.page_height,
            : 0,
            "stamp_y"//location_data.stamp_x,
            : 0,
            "stamp_width"//location_data.stamp_y,
            : 0,
            "stamp_height"//location_data.stamp_width,
            : 0,
            "logo_path"//location_data.stamp_height,
            : curr_profile.logo_path,
            "config_path": profilesModel.getConfigPath(),
            "cert_serial": curr_profile.cert_serial,
            "cert_serial_prefix": qsTr("Certificate: "),
            "cert_subject": cert_array[cert_index].subject_common_name,
            "cert_subject_prefix": qsTr("Subject: "),
            "cert_time_validity": qsTr("Vaildity: ")
                                  + cert_array[cert_index].not_before_readable + qsTr(
                " till ") + cert_array[cert_index].not_after_readable,
            "stamp_title": qsTr("THE DOCUMENT IS SIGNED WITH AN ELECTRONIC SIGNATURE"),
            "stamp_type": stamp_data.stamp_name,
            "text_color_red": stamp_data.text_color_red,
            "text_color_green": stamp_data.text_color_green,
            "text_color_blue": stamp_data.text_color_blue,
            "border_color_red": stamp_data.border_color_red,
            "border_color_green": stamp_data.border_color_green,
            "border_color_blue": stamp_data.border_color_blue,
            "border_width": stamp_data.border_width,
            "border_radius": stamp_data.border_radius,
            "bg_transparent": stamp_data.bg_transparent,
            "bg_opacity": 1,
            "cades_type": curr_profile.CADES_format,
            "tsp_url": curr_profile.tsp_url,
            "file_to_sign_path": pdfModel.getSource()
        }
        //console.warn(JSON.stringify(params))
        return params
    }

    function createPreview() {
        processing = true
        let params = setStampData()
        stampPreview.createImage(params)
    }

    PreviewRender {
        id: stampPreview
        width: parent.width
        visible: false

        Connections {

            function onImageReady() {
                if (stampPreview.visible === true) {
                    stampPreview.update()
                } else {
                    stampPreview.visible = true
                }
                processing = false
                if (new_requested) {
                    new_requested = false
                    createPreview()
                }
            }
        }
    }

    onStamp_dataChanged: {
        if (processing) {
            new_requested = true
        } else {
            createPreview()
        }
    }

    Component.onCompleted: {
        window_completed = true
    }
}
