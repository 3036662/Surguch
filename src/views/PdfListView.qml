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
    property int lastPageHeight: 0
    property int lastPageWidth: 0
    property int lastPageUsedSize: 0
    property double prevZoom: 1
    property bool landscape: false
    property bool zoomAuto: false
    property int delegateRotation: 0
    property int pageIndToPreserveWhenZoom: 0
    readonly property double maxZoom: 3
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
        prevZoom = zoomPageFact
        forceActiveFocus()
        if (zoomAuto) {
            let zoom_fact_goal = currentPage().zoomLast + 0.2
            zoomPageFact = zoom_fact_goal
            zoomAuto = false
            return
        }

        if (zoomPageFact < maxZoom) {
            zoomPageFact += 0.2
            if (zoomPageFact > minZoom) {
                canZoomOut()
            }
            if (zoomPageFact >= maxZoom) {
                maxZoomReached()
            }
        }
    }

    function zoomInWheel() {
        prevZoom = zoomPageFact
        let step = 0.10
        forceActiveFocus()
        if (zoomAuto) {
            let zoom_fact_goal = currentPage().zoomLast + step
            if (zoom_fact_goal < root.maxZoom) {
                zoomPageFact = zoom_fact_goal
                zoomAuto = false
            }
            return
        }

        if (zoomPageFact < maxZoom) {
            zoomPageFact += step
            if (zoomPageFact > minZoom) {
                canZoomOut()
            }
            if (zoomPageFact >= maxZoom) {
                maxZoomReached()
            }
        }
    }

    function zoomOut() {
        prevZoom = zoomPageFact
        forceActiveFocus()
        if (zoomAuto) {
            let zoom_fact_goal = currentPage().zoomLast - 0.2
            if (zoom_fact_goal <= 0) {
                return
            }
            zoomPageFact = zoom_fact_goal
            zoomAuto = false
            return
        }
        if (zoomPageFact > minZoom) {
            zoomPageFact -= 0.2
            if (zoomPageFact < maxZoom) {
                canZoom()
            }
            if (zoomPageFact < minZoom) {
                zoomPageFact = minZoom
                minZoomReached()
            }
        }
    }

    function zoomOutWheel() {
        prevZoom = zoomPageFact
        let step = 0.10
        forceActiveFocus()
        if (zoomAuto) {
            let zoom_fact_goal = currentPage().zoomLast - step
            if (zoom_fact_goal > minZoom) {
                zoomPageFact = zoom_fact_goal
                zoomAuto = false
            }
            return
        }
        if (zoomPageFact > minZoom) {
            zoomPageFact -= step
            if (zoomPageFact < maxZoom) {
                canZoom()
            }
            if (zoomPageFact <= minZoom) {
                minZoomReached()
                zoomPageFact = minZoom
            }
        }
    }

    function setZoom(newZoom) {
        prevZoom = zoomPageFact
        forceActiveFocus()
        if (newZoom <= 0) {
            //auto zoom
            zoomPageFact = -1
            zoomAuto = true
            canZoom()
            canZoomOut()
            return
        }
        zoomAuto = false
        zoomPageFact = newZoom / 100
        if (zoomPageFact < maxZoom) {
            canZoom()
        }
        if (zoomPageFact > minZoom) {
            canZoomOut()
        }     
    }

    function scrollToPage(newIndex) {
        forceActiveFocus()
        positionViewAtIndex(newIndex - 1, ListView.Beginning)
    }

    function reserRotation() {
        forceActiveFocus()
        if (delegateRotation !== 0) {
            delegateRotation = 0
            model.redrawAll()
            scrollToPage(currentPageIndex() + 1)
        }
    }

    function rotateClockWise() {
        forceActiveFocus()
        let currentPage = currentPageIndex() + 1
        delegateRotation = delegateRotation == 270 ? 0 : delegateRotation + 90
        model.redrawAll()
        scrollToPage(currentPage)
    }

    function rotateCounterClockWise() {
        forceActiveFocus()
        let currentPage = currentPageIndex() + 1
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

    function openTmpFile(file) {
        sourceIsTmp = true
        source = file
    }

    function openFile(file) {
        sourceIsTmp = false
        source = file
    }

    function saveTo(dest) {
        if (dest) {
            // The second parameter will let the model delete the source file.
            if (model.saveCurrSourceTo(dest, sourceIsTmp)) {
                openFile(dest)
            }
        }
    }

    function showInFolder() {
        if (source.length > 0) {
            model.showInFolder()
        }
    }


    /* Get the current view position at page: page index, ratio.
     * The ratio value means how far away the view is from the beginning of the page.
     * Returns object {value,ratio}
     */
    function preservePosition() {
        let viewMidY = contentY + height / 2
        let viewMidX = width / 2
        let currPage = itemAtIndex(pageIndToPreserveWhenZoom)
        let pageHeight = currPage ? currPage.height : root.lastPageHeight
        let pageLastZoom = currPage ? currPage.zoomLast : 1
        let pageYRatio = 1
        if (pageHeight > 0) {
            let a = 0
            let b = 0
            let step = pageHeight / 8
            let pointed = pageIndToPreserveWhenZoom
            let iterCounter = 0
            const maxIter = 10
            // measure the page in to directions (up and down) from the center
            while (pointed === pageIndToPreserveWhenZoom
                   && iterCounter < maxIter) {
                ++a
                pointed = indexAt(viewMidX, viewMidY - step * a)
            }
            pointed = pageIndToPreserveWhenZoom
            iterCounter = 0
            while (pointed === pageIndToPreserveWhenZoom
                   && iterCounter < maxIter) {
                ++b
                pointed = indexAt(viewMidX, viewMidY + step * b)
            }
            pageYRatio = a + b > 0 ? a / (a + b) : 0.5
        }
        let pos = {
            "index": pageIndToPreserveWhenZoom,
            "ratio": pageYRatio,
            "zoom_last": pageLastZoom
        }
        return pos
    }

    function currentPage() {
        let currPage = itemAt(width / 2, contentY + height / 2)
        let iterCount = 0
        while (currPage === null && iterCount < 10) {
            ++iterCount
            currPage = itemAt(width / 2,
                              contentY + height / 2 - spacing * iterCount)
        }
        return currPage
    }

    function currentPageIndex() {
        let index = indexAt(width / 2, contentY + height / 2)
        let iterCount = 0
        while (index === -1 && iterCount < 10) {
            ++iterCount
            index = indexAt(width / 2,
                            contentY + height / 2 - spacing * iterCount)
        }
        return index
    }

    Layout.fillHeight: true
    Layout.fillWidth: true
    Layout.leftMargin: 5
    Layout.rightMargin: 5
    Layout.minimumWidth: 200
    Layout.alignment: Qt.AlignHCenter

    spacing: 30
    flickableDirection: Flickable.HorizontalAndVerticalFlick
    clip: true
    focus: true
    keyNavigationEnabled: false

    onPageWidthChanged: {
        pageWidthUpdate(pageWidth)
        contentWidth = pageWidth
    }

    onSourceChanged: {
        lastPageHeight = 0;
        lastPageWidth = 0;
        lastPageUsedSize =0;
        prevZoom = 1;
        landscape = false;
        delegateRotation = 0;
        pageIndToPreserveWhenZoom =0;
        pdfModel.setSource(source)
        if (sourceIsTmp) {
            pdfModel.deleteFileLater(source)
        }
        setZoom(-1)        
        if (leftSideBar.sigCount === 0) {
            leftSideBar.showPreviews()
        } else {
            leftSideBar.showCerts()
        }
        aimResizeX = 1
        aimResizeY = 1
        aimIsAlreadyResized = false
        signInProgress = false
        if (source.length > 0) {
            root_window.title = source
        }
        scrollToPage(1);
        currPageChanged(1)
        forceActiveFocus()        
    }

    onZoomPageFactChanged: {        
        // preserve the position
        let pos = preservePosition()
        pdfModel.redrawAll()
        zoomFactorUpdate(zoomPageFact)
        positionViewAtIndex(pos.index, ListView.Beginning)
        // move the contentY to the old position
        let currPage = currentPage()
        let usedPageSize = 0
        let rotated90 = delegateRotation == 90 || delegateRotation == 270
        let currZoom = zoomPageFact
        if (currPage) {
            usedPageSize = rotated90 ? currPage.pWidth : currPage.pHeight
            if (currPage.zoomLast > 0 && currPage.zoomLast !== 1) {
                currZoom = currPage.zoomLast
            }
        } else {
            usedPageSize = rotated90 ? root.lastPageWidth : root.lastPageHeight
        }
        let zoomRatio = currZoom / pos.zoom_last
        let pos_mode = ListView.Beginning
        if (pos.ratio > 0.7) {
            pos_mode = ListView.End
        } else if (pos.ratio > 0.3) {
            pos_mode = ListView.Center
        }
        let targetYScroll = 0
        if (zoomRatio > 0) {
            targetYScroll = pos.ratio * usedPageSize
                    * (rotated90 && zoomRatio>1 ? 1 : zoomRatio) - root.height / 2
        }
        // Convert the negative value to a positive scroll from the previous page.
        if (targetYScroll < 0) {
            if (pos.index > 0) {
                positionViewAtIndex((pos.index) - 1, ListView.Beginning)
                targetYScroll += usedPageSize * zoomRatio + root.spacing
            }
        }
        if (targetYScroll > 0) {
            contentY += targetYScroll
        } else {
            // if failed to calculate the exact scroll, use jump mode ( beginning | middle | end )
            positionViewAtIndex(pos.index, pos_mode)
        }
        root.lastPageUsedSize = usedPageSize
    }

    onFlickEnded: {
        hScrollUpdate(contentX)
    }

    onCountChanged: {
        pagesCountChanged(count)
    }

    onContentYChanged: {
        var currentIndexAtTop = currentPageIndex()
        if (currentIndexAtTop > -1) {
            pageIndToPreserveWhenZoom = currentIndexAtTop
            currPageChanged(currentIndexAtTop + 1)
        }
    }

    model: pdfModel

    delegate: Column {
        width: root.width - verticalScroll.width
        property alias zoomLast: pdfPage.zoomLast
        property alias pWidth: pdfPage.width
        property alias pHeight: pdfPage.height

        PdfPageRender {
            id: pdfPage

            property int aimResizeStatus: root.aimIsAlreadyResized
            property bool sizeKnown: false
            property int defaultWidth: root.pageWidth > 0
                                       && !sizeKnown ? root.pageWidth : root.width

            customRotation: root.delegateRotation
            anchors.horizontalCenter: width < parent.width ? parent.horizontalCenter : undefined
            anchors.rightMargin: verticalScroll.width
            width: defaultWidth
            height: defaultWidth * 1.42
            // utilized,if zoomAuto == false
            zoomGoal: zoomPageFact
            // set goal width only if autoZoom; if autoZoom==true,zoomGoal will be ignored
            widthGoal: zoomAuto ? root.width : 0
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
                if (width > 0) {
                    lastPageWidth = width
                }
            }

            onZoomLastChanged: {
                // size is known after render
                sizeKnown = true
            }

            onHeightChanged: {
                updateCrossSize()
                landscape = pdfPage.width > pdfPage.height
                if (height > 0) {
                    root.lastPageHeight = height
                }
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
                enabled: !aimMouseArea.enabled
                anchors.fill: parent
                onClicked: {
                    root.forceActiveFocus()
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
                        root.forceActiveFocus()
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
        minimumSize: 0.2
        policy: ScrollBar.AsNeeded // Show scrollbar always
        snapMode: ScrollBar.NoSnap
    }

    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.NoButton

        Connections {
            function onWheel(event) {
                if (event.modifiers === Qt.ControlModifier) {
                    if (event.angleDelta.y > 0) {
                        zoomInWheel()
                    } else {
                        zoomOutWheel()
                    }
                    event.accepted = true
                    return
                }
                ;
                event.accepted = false
            }
        }
    }

    Keys.onPressed: event => {
                        if (event.key === Qt.Key_Left) {
                            flick(300, 0)
                            return
                        }
                        if (event.key === Qt.Key_Right) {
                            flick(-300, 0)
                        }
                        if (event.key === Qt.Key_Up) {
                            flick(0, 300)
                            return
                        }
                        if (event.key === Qt.Key_Down) {
                            flick(0, -300)
                        }
                        if (event.key === Qt.Key_P
                            && event.modifiers === Qt.ControlModifier) {
                            printer.print(pdfListView.source,
                                          pdfListView.count,
                                          pdfListView.landscape)
                        }
                        let currentIndexAtTop = currentPageIndex() + 1
                        if (!currentIndexAtTop) {
                            return
                        }
                        if (event.key === Qt.Key_PageUp
                            //|| event.key === Qt.Key_Left
                            || event.nativeScanCode === 112) {
                            if (currentIndexAtTop > 0) {
                                scrollToPage(currentIndexAtTop - 1)
                            }
                            event.accepted = true
                            return
                        }
                        if (event.key === Qt.Key_PageDown
                            || event.key === Qt.Key_Space
                            //|| event.key === Qt.Key_Right
                            || event.nativeScanCode === 117) {
                            scrollToPage(currentIndexAtTop + 1)
                            event.accepted = true
                            return
                        }
                    }
}
