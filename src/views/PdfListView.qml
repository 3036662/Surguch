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
    readonly property double maxZoom : 4
    readonly property double minZoom : 0.2

    signal pagesCountChanged(int count)
    signal currPageChanged(int index)
    signal pageWidthUpdate(int width)
    signal zoomFactorUpdate(double zoom)
    signal hScrollUpdate(int posX);
    signal maxZoomReached();
    signal canZoom();
    signal minZoomReached();
    signal canZoomOut();

    function zoomIn() {
        if (zoomPageFact < maxZoom) {
            if(zoomPageFact<=minZoom){
                canZoomOut();
            }
            zoomPageFact += 0.2
            if (zoomPageFact>=maxZoom){
                maxZoomReached();
            }
        }
    }

    function zoomOut() {
        if (zoomPageFact > minZoom) {
            if (zoomPageFact>=maxZoom){
                canZoom();
            }
            zoomPageFact -= 0.2
            if (zoomPageFact<minZoom){
                zoomPageFact=minZoom
            }
            if (zoomPageFact==minZoom){
                minZoomReached();
            }
        }
    }

    function setZoom(newZoom) {
        if (!newZoom > 0) {
            zoomPageFact = 1
            return
        }
        zoomPageFact = newZoom / 100
        console.warn("new zoom " + zoomPageFact)
    }

    onPageWidthChanged: {
        pageWidthUpdate(pageWidth)
        contentWidth=pageWidth
    }

    onSourceChanged: {
        pdfModel.setSource(source)
    }

    onZoomPageFactChanged: {
        zoomFactorUpdate(zoomPageFact)
    }

    onFlickEnded:{
        hScrollUpdate(contentX)
    }

    onCountChanged: {
        pagesCountChanged(count)
    }

    onCurrentIndexChanged: {
        currPageChanged(currentIndex)
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

        onWidthChanged: {
            root.pageWidth = width
        }

        Component.onCompleted: {
            setCtx(pdfModel.getCtx())
            setDoc(pdfModel.getDoc())
            setPageNumber(model.display)
            if (width>0 && root.hScrollPos>0 && root.hScrollPos<1){
                root.contentX = width * root.hScrollPos
            }
        }
    }

    ScrollBar.vertical: ScrollBar {
        width: 30
        policy: ScrollBar.AlwaysOn // Show scrollbar always
    }
}
