import QtQuick
import QtQuick.Controls
import alt.pdfcsp.pdfModel
import alt.pdfcsp.pdfRender

ListView {
    id: root

    property int pageWidth: 0
    property double hScrollPos: 0
    property string source: ""
    property double zoomPageFact: 1

    signal pagesCountChanged(int count)
    signal currPageChanged(int index)
    signal pageWidthUpdate(int width)

    function zoomIn() {
        if (zoomPageFact < 4) {
            zoomPageFact += 0.2
        }
    }

    function zoomOut() {
        if (zoomPageFact > 0.4) {
            zoomPageFact -= 0.2
            console.warn(zoomPageFact)
        }
    }

    onPageWidthChanged: {
        pageWidthUpdate(pageWidth)
    }

    onSourceChanged: {
        pdfModel.setSource(source)
    }

    anchors.horizontalCenter: parent.horizontalCenter
    anchors.fill: parent
    anchors.leftMargin: 20
    spacing: 30
    anchors.rightMargin: 20
    flickableDirection: Flickable.HorizontalAndVerticalFlick

    model: MuPdfModel {
        id: pdfModel
    }

    delegate: PdfPageRender {
        id: pdfPage
        width: root.width * zoomPageFact
        height: width * 1.42

        Component.onCompleted: {
            setCtx(pdfModel.getCtx())
            setDoc(pdfModel.getDoc())
            setPageNumber(model.display)
            root.contentX = width * root.hScrollPos
        }

        onWidthChanged: {
            root.pageWidth = width
        }
    }

    ScrollBar.vertical: ScrollBar {
        width: 30
        policy: ScrollBar.AlwaysOn // Show scrollbar always
    }

    onCountChanged: {
        pagesCountChanged(count)
    }
    onCurrentIndexChanged: {
        currPageChanged(currentIndex)
    }
}
