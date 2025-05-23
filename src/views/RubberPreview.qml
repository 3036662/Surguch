import QtQuick
import alt.pdfcsp.rubberPreviewRender

Item {
    id: root

    property var image_data
    property var stamp_data
    property bool processing: false
    property bool new_requested: false
    property bool window_completed: false

    function setStampData() {

    }


    function createPreview() {
        processing = true
        let params = setStampData()
        rubberPreviewRender.createImage(stamp_data)
    }

    RubberPreviewRender {
        id: rubberPreviewRender
        width: parent.width
        visible: false

        Connections {

            function onImageReady() {
                if (rubberPreviewRender.visible === true) {
                    rubberPreviewRender.update()
                } else {
                    rubberPreviewRender.visible = true
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