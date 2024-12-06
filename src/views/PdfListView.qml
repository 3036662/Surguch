import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import alt.pdfcsp.pdfModel
import alt.pdfcsp.pdfRender

ListView {
    id: root

    property double hScrollPos: 0
    // --------
    // source
    property string source: ""
    property bool sourceIsTmp: false // the source file is temporary
    // --------
    // page sizes and zoom
    property double zoomPageFact: 1
    property int pageWidth: 0
    property bool landscape: false
    property bool zoomAuto: false
    property int delegateRotation: 0
    readonly property double maxZoom: 4
    readonly property double minZoom: 0.2
    // --------
    //signing
    property bool signMode: false
    property bool signInProgress: false
    // --------
    //aim
    property double aimResizeX: 1
    property double aimResizeY: 1
    property bool aimIsAlreadyResized: false
    property bool aimResizeInProgress: false

    signal pagesCountChanged(int count)
    signal currPageChanged(int index)
    signal pageWidthUpdate(int width)
    signal zoomFactorUpdate(double zoom)
    signal hScrollUpdate(int posX)
    signal maxZoomReached
    signal canZoom
    signal minZoomReached
    signal canZoomOut
    signal stampLocationSelected(var stamp_location_info)

    function zoomIn() {
        if (zoomAuto){
            zoomAuto=false;
            zoomPageFact=1;
        }
        if (zoomPageFact < maxZoom) {
            if (zoomPageFact <= minZoom) {
                canZoomOut()
            }
            zoomPageFact += 0.2
            if (zoomPageFact >= maxZoom) {
                maxZoomReached()
            }
        }
    }

    function zoomOut() {
        if (zoomAuto){
            zoomAuto=false;
            zoomPageFact=1;
        }
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
        if (newZoom <= 0) { //auto zoom
            zoomPageFact = -1
            zoomAuto=true;
            return
        }
        zoomAuto=false;
        zoomPageFact = newZoom / 100
        if (zoomPageFact < maxZoom) {
            canZoom()
        }
        if (zoomPageFact > minZoom) {
            canZoomOut()
        }
        console.warn("new zoom "+zoomPageFact);
    }

    function scrollToPage(newIndex) {
        positionViewAtIndex(newIndex - 1, ListView.Beginning)
    }

    function reserRotation() {
        if (delegateRotation !== 0) {
            delegateRotation = 0
            model.redrawAll()
            scrollToPage(indexAt(100, contentY + 100) + 1)
        }
    }

    function rotateClockWise() {
        let currentPage = indexAt(100, contentY + 100) + 1
        delegateRotation = delegateRotation == 270 ? 0 : delegateRotation + 90
        model.redrawAll()
        scrollToPage(currentPage)
    }

    function rotateCounterClockWise() {
        let currentPage = indexAt(100, contentY + 100) + 1
        delegateRotation = delegateRotation == 0 ? 270 : delegateRotation - 90
        model.redrawAll()
        scrollToPage(currentPage)
    }

    function updateStampResizeFactor(data) {
        aimResizeX = data.x_resize !== undefined ? data.x_resize : 1
        aimResizeY = data.y_resize !== undefined ? data.y_resize : 1
        aimIsAlreadyResized = true
        aimResizeInProgress = false
        console.warn("Finished stamp size calculation")
    }

    function openTmpFile(file){
        sourceIsTmp=true;
        source=file
    }

    function openFile(file){
        sourceIsTmp=false;
        source=file
    }

    function saveTo(dest){
        if (dest){
            // The second parameter will let the model delete the source file.
            if (model.saveCurrSourceTo(dest,sourceIsTmp)){
                openFile(dest)
            }
        }
    }

    function showInFolder(){
        if (source.length>0){
            model.showInFolder();
        }
    }

    Layout.fillHeight: true
    Layout.fillWidth: true
    Layout.leftMargin: 20
    Layout.rightMargin: 20
    Layout.minimumWidth: 200
    Layout.alignment: Qt.AlignHCenter

    spacing: 30
    flickableDirection: Flickable.HorizontalAndVerticalFlick
    clip: true

    onPageWidthChanged: {
        pageWidthUpdate(pageWidth)
        contentWidth = pageWidth
    }

    onSourceChanged: {
        pdfModel.setSource(source)
        if (sourceIsTmp){
            pdfModel.deleteFileLater(source)
        }
        setZoom(-1)

        delegateRotation = 0
        if (leftSideBar.sigCount === 0) {
            leftSideBar.showPreviews()
        } else {
            leftSideBar.showCerts()
        }
        aimResizeX = 1
        aimResizeY = 1
        aimIsAlreadyResized = false
        signInProgress = false
        if (source.length>0){
            root_window.title=source;
        }
    }

    onZoomPageFactChanged: {
        zoomFactorUpdate(zoomPageFact)
        pdfModel.redrawAll()
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

    onZoomAutoChanged: {
        pdfModel.redrawAll()
    }


    model: pdfModel

    delegate: Column {
        width: root.width - verticalScroll.width * 2

        PdfPageRender {
            id: pdfPage



            property int customRotation: root.delegateRotation
            property int aimResizeStatus: root.aimIsAlreadyResized

            anchors.horizontalCenter: parent.horizontalCenter
            anchors.rightMargin: verticalScroll.width

            zoomGoal: zoomPageFact
            // set goal width only if autoZoom
            widthGoal: zoomAuto ? root.width : 0;
            currScreenDpi: pdfModel.screenDpi

            function updateCrossSize() {
                if (!root.aimIsAlreadyResized && pdfPage.width > 0
                        && pdfPage.height > 0) {
                    cross.width = pdfPage.width
                            < pdfPage.height ? Math.round(
                                                   pdfPage.width * 0.41) : Math.round(
                                                   pdfPage.width * 0.3)
                    if (pdfPage.height != 0) {
                        cross.height = pdfPage.width
                                < pdfPage.height ? Math.round(
                                                       pdfPage.height / 9) : Math.round(
                                                       pdfPage.height / 7)
                    }
                    // run background estimate of stamp size
                    if (!aimResizeInProgress) {
                        let location_data = {
                            "page_index": index,
                            "page_width": width,
                            "page_height": height,
                            "stamp_x": cross.x,
                            "stamp_y": cross.y,
                            "stamp_width": cross.width,
                            "stamp_height": cross.height
                        }
                        console.warn("Call stamp size calculation")
                        aimResizeInProgress = true
                        sigCreator.resizeAim(location_data)
                    }
                } else {
                    // if the aim is already resized - update with resize factor
                    cross.width = pdfPage.width
                            < pdfPage.height ? Math.round(
                                                   pdfPage.width * 0.41 * aimResizeX) : Math.round(
                                                   pdfPage.width / 3 * aimResizeX)
                    cross.height = pdfPage.width
                            < pdfPage.height ? Math.round(
                                                   pdfPage.height / 9 * aimResizeY) : Math.round(
                                                   pdfPage.height / 7 * aimResizeY)
                }
            }

            onWidthChanged: {
                root.pageWidth = width                
                updateCrossSize()
            }

            onHeightChanged: {
                updateCrossSize()
                landscape=pdfPage.width>pdfPage.height;
            }

            onAimResizeStatusChanged: {
                updateCrossSize()
            }

            Component.onCompleted: {
                setCtx(pdfModel.getCtx())
                setDoc(pdfModel.getDoc())
                setPageNumber(model.display)
                if (width > 0 && root.hScrollPos > 0 && root.hScrollPos < 1) {
                    root.contentX = width * root.hScrollPos
                }                
            }

            MouseArea {
                id: aimMouseArea

                enabled: root.signMode || root.signInProgress
                anchors.fill: parent
                hoverEnabled: true

                onEntered: {
                    cross.visible = root.signInProgress ? false : true
                    cursorShape = root.signInProgress ? Qt.BusyCursor : Qt.CrossCursor
                }
                onExited: {
                    cross.visible = false
                    cursorShape = Qt.ArrowCursor
                }
                onClicked: {
                    if (root.signMode && !root.signInProgress
                            && cross.valid_position) {
                        let location_data = {
                            "page_index": index,
                            "page_width": width,
                            "page_height": height,
                            "stamp_x": cross.x,
                            "stamp_y": cross.y,
                            "stamp_width": cross.width,
                            "stamp_height": cross.height
                        }
                        cross.visible = false
                        cursorShape = Qt.BusyCursor
                        root.signMode = false
                        root.signInProgress = true
                        stampLocationSelected(location_data)
                    }
                }

                onPositionChanged: {
                    cross.x = mouseX - cross.width / 2
                    cross.y = mouseY - cross.height / 2
                    if (cross.x < 0 || cross.x + cross.width > pdfPage.width
                            || cross.y < 0
                            || cross.y + cross.height > pdfPage.height) {
                        cross.valid_position = false
                    } else {
                        cross.valid_position = true
                    }
                }

                Rectangle {
                    id: cross

                    property string defaultText: qsTr("Stamp position")
                    property string invalidPositionText: qsTr(
                                                             "Invalid position")
                    property bool valid_position: true

                    width: 0
                    height: 0
                    color: "transparent"
                    border.color: valid_position ? "blue" : "red"
                    border.width: 2
                    visible: false

                    Text {
                        topPadding: 10
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: cross.valid_position ? cross.defaultText : cross.invalidPositionText
                        color: cross.valid_position ? "blue" : "red"
                        font.family: "Noto Sans"
                    }
                }
            }
        }
    }

    ScrollBar.vertical: ScrollBar {
        id: verticalScroll
        width: 15
        minimumSize:0.2
        policy:  ScrollBar.AsNeeded// Show scrollbar always
        snapMode: ScrollBar.NoSnap
    }
}
