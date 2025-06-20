import QtQuick
import alt.pdfcsp.rubberPreviewRender

Item {
    id: root

    property var stamp_data
    property bool processing: false
    property bool new_requested: false
    property bool window_completed: false

    function setStampData() {
        if (!stamp_data) {
            return {}
        }
        //let curr_stamp = JSON.parse(stamp_data)
        let params = {
            "stamp_width": 900,
            "stamp_height": 300,
            "create_from_image": stamp_data.create_from_image,
            "img_path": stamp_data.img_path,
            "border_width": stamp_data.border_width,
            "border_radius": stamp_data.border_radius,
            "text_color_red": stamp_data.text_color_red,
            "text_color_green": stamp_data.text_color_green,
            "text_color_blue": stamp_data.text_color_blue,
            "border_color_red": stamp_data.border_color_red,
            "border_color_green": stamp_data.border_color_green,
            "border_color_blue": stamp_data.border_color_blue,
            "bg_color_red": stamp_data.bg_color_red,
            "bg_color_green": stamp_data.bg_color_green,
            "bg_color_blue": stamp_data.bg_color_blue,
            "font_family": stamp_data.font_family,
            "annotation_text": stamp_data.annotation_text,
            "bg_transparent": stamp_data.bg_transparent,
            "annotation_width": stamp_data.annotation_width
        }
        return params
    }


    function createPreview() {
        processing = true
        let params = setStampData()
        rubberPreview.createImage(params)
    }

    Image {
        anchors.fill: parent
        source: "qrc:/pb.jpg"
    }

    RubberPreviewRender {
        id: rubberPreview
        width: parent.width
        visible: false

        Connections {

            function onImageReady() {
                if (rubberPreview.visible === true) {
                    rubberPreview.update()
                } else {
                    rubberPreview.visible = true
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