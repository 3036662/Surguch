import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import alt.pdfcsp.pdfModel
import alt.pdfcsp.pdfRender

ListView {
    id: root
    property int pageWidth: 0
    property double hScrollPos: 0
    property string source: ""
    property double zoomPageFact: 1
    property int delegateRotation: 0

    readonly property double maxZoom: 4
    readonly property double minZoom: 0.2

    signal pagesCountChanged(int count)
    signal currPageChanged(int index)
    signal pageWidthUpdate(int width)
    signal zoomFactorUpdate(double zoom)
    signal hScrollUpdate(int posX)
    signal maxZoomReached
    signal canZoom
    signal minZoomReached
    signal canZoomOut

    Layout.fillHeight: true
    Layout.fillWidth: true
    Layout.leftMargin: 20
    Layout.rightMargin: 20
    Layout.minimumWidth:  200
    Layout.alignment: Qt.AlignHCenter

    spacing: 30
    flickableDirection: Flickable.HorizontalAndVerticalFlick
    Layout.horizontalStretchFactor: 4
    clip: true

    function zoomIn() {
        if (zoomPageFact < maxZoom) {
            if (zoomPageFact <= minZoom) {
                canZoomOut()
            }
            zoomPageFact += 0.2
            if (zoomPageFact >= maxZoom) {
                maxZoomReached()
            }
        }
        console.warn("X=" + contentX)
    }

    function zoomOut() {
        if (zoomPageFact > minZoom) {
            if (zoomPageFact >= maxZoom) {
                canZoom()
            }
            zoomPageFact -= 0.2
            if (zoomPageFact < minZoom) {
                zoomPageFact = minZoom
            }
            if (zoomPageFact == minZoom) {
                minZoomReached()
            }
        }
    }

    function setZoom(newZoom) {
        if (!newZoom > 0) {
            zoomPageFact = 1
            return
        }
        zoomPageFact = newZoom / 100
        //console.warn("new zoom " + zoomPageFact)
    }

    function scrollToPage(newIndex) {
        positionViewAtIndex(newIndex - 1, ListView.Beginning)
    }

    function rotateClockWise() {
        delegateRotation = delegateRotation == 270 ? 0 : delegateRotation + 90
        model.redrawAll()
    }

    function rotateCounterClockWise() {
        delegateRotation = delegateRotation == 0 ? 270 : delegateRotation - 90
        model.redrawAll()
    }

    onPageWidthChanged: {
        pageWidthUpdate(pageWidth)
        contentWidth = pageWidth
    }

    onSourceChanged: {
        pdfModel.setSource(source)
        setZoom(100)
        delegateRotation = 0
    }

    onZoomPageFactChanged: {
        zoomFactorUpdate(zoomPageFact)
    }

    onFlickEnded: {
        hScrollUpdate(contentX)
    }

    onCountChanged: {
        pagesCountChanged(count)
    }

    onContentYChanged: {
        var currentIndexAtTop = indexAt(50, contentY)
        if (currentIndexAtTop > -1) {
            currPageChanged(currentIndexAtTop + 1)
        }
    }

    model: MuPdfModel {
        id: pdfModel
    }

    delegate: Column {
        width: root.width
        PdfPageRender {
            id: pdfPage
            width: root.width * zoomPageFact
            height: width * 1.42
            //anchors.horizontalCenter: parent.horizontalCenter
            property int customRotation: root.delegateRotation

            onWidthChanged: {
                root.pageWidth = width
            }

            Component.onCompleted: {
                setCtx(pdfModel.getCtx())
                setDoc(pdfModel.getDoc())
                setPageNumber(model.display)
                if (width > 0 && root.hScrollPos > 0 && root.hScrollPos < 1) {
                    root.contentX = width * root.hScrollPos
                }
            }
        }
    }

    ScrollBar.vertical: ScrollBar {
        width: 30
        policy: ScrollBar.AlwaysOn // Show scrollbar always
    }
}
