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

    function createPreview() {
        if (!(profile_data && stamp_json)) {
            return
        }
        try {
            let params = sigCreatorWrapper.gatherParams(null, null,
                                                        profile_data,
                                                        stamp_json)
            private_data.processing = true
            //console.warn(JSON.stringify(params))
            stampPreview.createImage(params)
        } catch (e) {
            console.warn("[createPreview] error:" + e)
        }
    }

    Image {
        id: bgImage

        anchors.fill: parent
        source: "qrc:/chess_bg.jpg"

        PreviewRender {
            id: stampPreview
            width: parent.width
            visible: false

            maxWidth: parent.width
            maxHeight: parent.height

            // initial position
            x: bgImage.width / 2 - width / 2
            y: bgImage.height / 2 - height / 2

            onWidthChanged: {
                x = bgImage.width / 2 - width / 2
            }

            onHeightChanged: {
                y = bgImage.height / 2 - height / 2
            }

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
            onStampPreviewBadResult: {
                console.warn("[stampPreview] render failed")
                stampPreview.visible = false
                private_data.processing = false
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
