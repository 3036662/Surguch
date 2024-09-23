import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import alt.pdfcsp.pdfModel
import alt.pdfcsp.pdfRender

ColumnLayout {
    id: root
    property string source: ""
    Layout.maximumWidth: 400
    Layout.fillHeight: true
    Layout.verticalStretchFactor: 1

    signal pageClick(int index)

    ListView {
        id: previewListView
        Layout.leftMargin: 10
        //Layout.rightMargin: 10
        Layout.fillHeight: true
        Layout.fillWidth: true
        Layout.alignment: Qt.AlignHCenter
        spacing: 10
        flickableDirection: Flickable.VerticalFlick

        property int delegateRotation: 0
        property double zoomPageFact: 1

        property alias source: root.source

        model: MuPdfModel {
            id: pdfPreviewModel
        }

        delegate: Column {
            width: previewListView.width

            PdfPageRender {
                id: pdfPreviewPage
                width: previewListView.width - verticalScroll.width * 2
                height: width * 1.42
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.rightMargin: verticalScroll.width

                Component.onCompleted: {
                    setCtx(pdfPreviewModel.getCtx())
                    setDoc(pdfPreviewModel.getDoc())
                    setPageNumber(model.display)
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        pageClick(index + 1)
                    }
                }
            }
        }

        onSourceChanged: {
            pdfPreviewModel.setSource(source)
        }

        ScrollBar.vertical: ScrollBar {
            id: verticalScroll
            width: 30
            policy: ScrollBar.AlwaysOn // Show scrollbar always
        }
    }

    function scrollToPage(newIndex) {
        previewListView.positionViewAtIndex(newIndex - 1, ListView.Beginning)
    }
}
