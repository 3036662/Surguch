import QtQuick
import alt.pdfcsp.rubberPreviewRender

Item {
    id: root

    // these fields are set from RubberPreviewLeftPanel.qml
    property var stamp_json // JSON
    property real imageWidth: 340
    property real imageHeight: 280


    // private properties
    Item{
     id: private_data
     visible:false

     property bool processing: false
     property bool new_requested: false
    }

    function getStampParams() {
        if (!stamp_json) {
            return {}
        }
        let params = {
            "stamp_width": 900,
            "stamp_height": 300,
            "create_from_image": stamp_json.create_from_image,
            "img_path": stamp_json.img_path,
            "border_width": stamp_json.border_width,
            "border_radius": stamp_json.border_radius,
            "text_color_red": stamp_json.text_color_red,
            "text_color_green": stamp_json.text_color_green,
            "text_color_blue": stamp_json.text_color_blue,
            "border_color_red": stamp_json.border_color_red,
            "border_color_green": stamp_json.border_color_green,
            "border_color_blue": stamp_json.border_color_blue,
            "bg_color_red": stamp_json.bg_color_red,
            "bg_color_green": stamp_json.bg_color_green,
            "bg_color_blue": stamp_json.bg_color_blue,
            "font_family": stamp_json.font_family,
            "annotation_text": stamp_json.annotation_text,
            "bg_transparent": stamp_json.bg_transparent,
            "annotation_width": stamp_json.annotation_width
        }
        return params
    }

    function createPreview() {
        private_data.processing = true
        let params = getStampParams()
        rubberPreview.createImage(params)
    }

    Image {
        id: bgImage
        width: root.imageWidth
        height: root.imageHeight
        anchors.horizontalCenter: parent.horizontalCenter
        source: "qrc:/chess_bg.jpg"

        property bool completed: false

        RubberPreviewRender {
            id: rubberPreview

            // true until the first successful render
            property bool first_launch: true
            // There is no sense in showing the item if the render failed.
            property bool bad_result_recieved: false

            // initial width
            width: root.imageWidth
            height: root.imageHeight

            // initial position
            x: bgImage.width / 2 - width / 2
            y: bgImage.height / 2 - height / 2

            // size hint for the renderer
            requestedWidth: root.imageWidth
            requestedHeight: root.imageHeight

            // visible only if we have an image to show, and no render is processing now
            visible: !(private_data.processing || first_launch || bad_result_recieved)

            onWidthChanged: {
                x = bgImage.width / 2 - width / 2
            }

            onHeightChanged: {
                y = bgImage.height / 2 - height / 2
            }

            // render succeeded
            onRubberImageReady: {
                private_data.processing = false
                bad_result_recieved = false

                if (first_launch) {
                    first_launch = false
                }

                rubberPreview.update()
                rubberPreview.y = bgImage.height / 2 - rubberPreview.height / 2
                rubberPreview.x = bgImage.width / 2 - rubberPreview.width / 2

                if (private_data.new_requested) {
                    private_data.new_requested = false
                    createPreview()
                }
            }

            // render failed
            onRubberBadResult: {
                private_data.processing = false
                bad_result_recieved = true
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
