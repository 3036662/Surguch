import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import alt.pdfcsp.pdfModel
import alt.pdfcsp.pdfRender

ListView {
    id: root

    property int delegateRotation: 0
    property double zoomPageFact: 1
    property string source: ""

    anchors.leftMargin: 5
    anchors.rightMargin: 0
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.fill: parent
    spacing: 10
    flickableDirection: Flickable.VerticalFlick

    model: MuPdfModel {
        id: pdfPreviewModel
    }

    delegate: Column {
        width: root.width

        PdfPageRender {
            visible: root.visible
            id: pdfPreviewPage

            width: root.width - verticalScroll.width * 2
            widthGoal: width


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
        width: 15
        minimumSize:0.2
        policy: ScrollBar.AsNeeded // Show scrollbar always
    }
}
