import QtQuick
import alt.pdfcsp.rubberPreviewRender

Item {
    id: root

    property var stamp_data
    property bool processing: false
    property bool new_requested: false
    property bool window_completed: false

    property real imageWidth: 340
    property real imageHeight: 280

    function setStampData() {
        if (!stamp_data) {
            return {}
        }
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
            visible: !(processing || first_launch || bad_result_recieved)

            onWidthChanged: {
                x = bgImage.width / 2 - width / 2
            }

            onHeightChanged: {
                y = bgImage.height / 2 - height / 2
            }

            // render succeeded
            onRubberImageReady: {
                processing = false
                bad_result_recieved = false

                if (first_launch) {
                    first_launch = false
                }

                rubberPreview.update()
                rubberPreview.y = bgImage.height / 2 - rubberPreview.height / 2
                rubberPreview.x = bgImage.width / 2 - rubberPreview.width / 2

                if (new_requested) {
                    new_requested = false
                    createPreview()
                }
            }

            // render failed
            onRubberBadResult: {
                processing = false
                bad_result_recieved = true
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
