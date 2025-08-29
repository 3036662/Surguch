import QtQuick
import alt.pdfcsp.previewRender

Item {
    id: root

    // these fields are set from StampEditor.qml
    property var profile_data
    // string(JSON) profile data
    property var stamp_json
    // JSON object stamp data

    //property var image_data // TODO(Oleg) is it unused ?

    // private properties
    Item {
        id: private_data
        visible: false

        property bool processing: false
        property bool new_requested: false
    }

    function getStampParams() {
        //console.warn("preview " + profile_data)
        // let curr_profile
        if (!profile_data || !stamp_json) {
            return {}
        }

        let curr_profile = JSON.parse(profile_data)
        let cert_array = JSON.parse(profilesModel.getUserCertsJSON())
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
            "stamp_type": stamp_json.stamp_name,
            "text_color_red": stamp_json.text_color_red,
            "text_color_green": stamp_json.text_color_green,
            "text_color_blue": stamp_json.text_color_blue,
            "border_color_red": stamp_json.border_color_red,
            "border_color_green": stamp_json.border_color_green,
            "border_color_blue": stamp_json.border_color_blue,
            "border_width": stamp_json.border_width,
            "border_radius": stamp_json.border_radius,
            "bg_transparent": stamp_json.bg_transparent,
            "bg_opacity": 1,
            "cades_type": curr_profile.CADES_format,
            "tsp_url": curr_profile.tsp_url,
            "file_to_sign_path": pdfModel.getSource()
        }
        //console.warn(JSON.stringify(params))
        return params
    }

    function createPreview() {
        private_data.processing = true
        let params = getStampParams()
        stampPreview.createImage(params)
    }

    Image {
        id: bgImage

        anchors.fill: parent
        source: "qrc:/chess_bg.jpg"

        PreviewRender {
            id: stampPreview
            width: parent.width
            visible: false

            onImageReady: {
                if (stampPreview.visible === true) {
                    stampPreview.update()
                } else {
                    stampPreview.visible = true
                }

                stampPreview.y = bgImage.height / 2 - stampPreview.height / 2
                stampPreview.x = bgImage.width / 2 - stampPreview.width / 2

                private_data.processing = false
                if (private_data.new_requested) {
                    private_data.new_requested = false
                    createPreview()
                }
            }
        }
    }

    onStamp_jsonChanged: {
        if (private_data.processing) {
            private_data.new_requested = true
        } else {
            createPreview()
        }
    }
}
