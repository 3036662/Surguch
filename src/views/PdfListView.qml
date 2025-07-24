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
    property int pageHeight: 0
    property int lastPageHeight: 0 // used for preservePos
    property int lastPageWidth: 0 // used for preservePos
    property int lastPageUsedSize: 0 // used for jumpToPosition
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
    //tag stamps
    property bool tagMode: false
    property bool tagInProgress: false
    property bool size_estimated: false
    property bool tag_placing: false
    property var tagData
    property var ratio
    property double startX
    property double startY
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

    signal quitSignMode

    signal disableTagMode

    signal stampLocationSelected(var stamp_location_info, var path)

    signal tagPlaced

    signal updateLSB(var source)

    signal updateHistory(int undo, int redo)

    function proceedSigning(location_data) {
        //console.warn(pdfModel.getSource())
        let tmpFile = pdfModel.getSource()
        if (tagInProgress) {
            //console.warn("embedding tags")
            tagInProgress = false
            tmpFile = tagCreator.embedAnnot(pdfModel.getAnnotParams(),
                                            pdfModel.getSource())
            pdfModel.deleteFileLater(tmpFile)
            //openTmpFile(tmpFile)
            //console.warn("NEW SOURCE AFTER EMBEDDING RUBBER STAMPS");
            //console.warn("new source: " + tmpFile)
        }
        //console.warn("starting to sign")
        signMode = false
        signInProgress = true
        stampLocationSelected(location_data, tmpFile)
        forceActiveFocus()
    }

    function undo() {
        pdfModel.undoRubberStamp()
        let undoCount = pdfModel.getUndoCount()
        let redoCount = pdfModel.getRedoCount()
        updateHistory(undoCount, redoCount)
    }

    function redo() {
        pdfModel.redoRubberStamp()
        let undoCount = pdfModel.getUndoCount()
        let redoCount = pdfModel.getRedoCount()
        updateHistory(undoCount, redoCount)
    }

    function zoomIn() {
        prevZoom = zoomPageFact
        tryToGetFocus()
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
        tryToGetFocus()
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
        tryToGetFocus()
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
        tryToGetFocus()
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
        tryToGetFocus()
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
        tryToGetFocus()
        positionViewAtIndex(newIndex - 1, ListView.Beginning)
    }

    function reserRotation() {
        tryToGetFocus()
        if (delegateRotation !== 0) {
            delegateRotation = 0
            model.redrawAll()
            scrollToPage(currentPageIndex() + 1)
        }
    }

    function rotateClockWise() {
        tryToGetFocus()
        let currentPage = currentPageIndex() + 1
        delegateRotation = delegateRotation == 270 ? 0 : delegateRotation + 90
        model.redrawAll()
        scrollToPage(currentPage)
    }

    function rotateCounterClockWise() {
        tryToGetFocus()
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

    function saveTo(file, dest) {
        if (dest) {
            // The second parameter will let the model delete the source file.
            if (model.saveCurrSourceTo(file, dest, sourceIsTmp)) {
                openFile(dest)
            }
        }
    }

    function showInFolder() {
        if (source.length > 0) {
            model.showInFolder()
        }
    }

    function tryToGetFocus() {
        if (root_window.focusOwnerId !== ""
                && root_window.focusOwnerId !== "searchDialog") {
            root_window.focusOwnerId = "pdfListView"
            forceActiveFocus()
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
        //console.warn("QML PreservsPos:" + JSON.stringify(pos))
        return pos
    }


    /*
     *   @brief Try to position the view inside the page
     *   @param pos should be object like this
     *
     *   pos = {
     *       "index": pageIndToPreserveWhenZoom,  - page index
     *       "ratio": pageYRatio,                 - float y position on page
     *       "zoom_last": pageLastZoom
     *   }
     */
    function jumpToPosition(pos) {
        //console.warn("jump to position:" + JSON.stringify(pos))
        positionViewAtIndex(pos.index, ListView.Beginning)
        let currPage = currentPage()
        let rotated90 = delegateRotation == 90 || delegateRotation == 270
        let currZoom = zoomPageFact
        let usedPageSize = 0
        let lastSizeUsed = false
        // console.warn("currPage.pWidth: " + currPage.pWidth
        //              + " currPage.pHeight: " + currPage.pHeight)
        if (currPage) {
            usedPageSize = rotated90 ? currPage.pWidth : currPage.pHeight
            if (currPage.zoomLast > 0 && currPage.zoomLast !== 1) {
                currZoom = currPage.zoomLast
            }
        } else {
            lastSizeUsed = true
            usedPageSize = rotated90 ? root.lastPageWidth : root.lastPageHeight
        }

        let zoomRatio = currZoom / pos.zoom_last
        if (zoomRatio < 0) {
            zoomRatio = 1
        }
        let pos_mode = ListView.Beginning
        if (pos.ratio > 0.7) {
            pos_mode = ListView.End
        } else if (pos.ratio > 0.3) {
            pos_mode = ListView.Center
        }
        let targetYScroll = 0
        if (zoomRatio > 0) {
            targetYScroll = pos.ratio * usedPageSize
            if (lastSizeUsed) {
                targetYScroll *= zoomRatio
                targetYScroll = 0
            } else {
                targetYScroll -= root.height / 2
            }
        }
        //console.warn("scrollY " + targetYScroll)
        if (targetYScroll > 0 && pos.index > 0) {
            //console.warn("targetYScroll > 0")
            //console.warn("pos index: " + pos.index)
            positionViewAtIndex(pos.index, ListView.Beginning)
            contentY += targetYScroll
        } else {
            // if failed to calculate the exact scroll, use jump mode ( beginning | middle | end )
            positionViewAtIndex(pos.index, pos_mode)
        }
        root.lastPageUsedSize = usedPageSize
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

    function searchCompleted(first_needle_page_index, total_needles, x_rel, y_rel) {
        //let curr_page= currentPageIndex();
        let pos = preservePosition()

        model.redrawAll()
        console.warn("QML Total needles:" + total_needles)
        if (total_needles > 0) {
            pos.index = first_needle_page_index
            pos = updateRatioWithRoration(pos, x_rel, y_rel)
        }

        jumpToPosition(pos)
    }

    function updateRatioWithRoration(pos, x_rel, y_rel) {
        switch (delegateRotation) {
        case 90:
            pos.ratio = x_rel
            if (pos.ratio > 0.7) {
                pos.ratio = 0.7
            }
            break
        case 270:
            pos.ratio = 1 - x_rel
            if (pos.ratio > 0.7) {
                pos.ratio = 0.7
            }
            break
        case 180:
            pos.ratio = 1 - y_rel
            break
        default:
            pos.ratio = y_rel
        }
        if (pos.ratio > 0.9) {
            pos.ratio = 0.9
        }
        return pos
    }

    function jumpToNeedle(page_index, rel_x, rel_y) {
        //console.warn("QML jump to needle on page " + page_index)
        let currPage = currentPage()
        // remove current rect from this page
        if (currentPageIndex() !== page_index) {
            currPage.updateCurrRect()
        }
        let pageLastZoom = currPage ? currPage.zoomLast : 1
        let pos = {
            "index": page_index,
            "ratio": rel_y,
            "zoom_last": pageLastZoom
        }
        pos = updateRatioWithRoration(pos, rel_x, rel_y)
        jumpToPosition(pos)
        currPage = root.itemAtIndex(page_index)
        //console.warn("QML update page at index " + page_index)
        // update current rect
        currPage.updateCurrRect()
    }

    // redraw but preserve the postiton
    function redrawAndPreservePosion() {
        let pos = preservePosition()
        model.redrawAll()
        jumpToPosition(pos)
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
        //console.warn("pdflistview source = " + source)
        lastPageHeight = 0
        lastPageWidth = 0
        lastPageUsedSize = 0
        prevZoom = 1
        landscape = false
        delegateRotation = 0
        pageIndToPreserveWhenZoom = 0
        pdfModel.setSource(source)
        tagInProgress = false
        if (sourceIsTmp) {
            pdfModel.deleteFileLater(source)
        }
        setZoom(100)
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
        scrollToPage(1)
        currPageChanged(1)
        tryToGetFocus()
        let undoCount = pdfModel.getUndoCount()
        let redoCount = pdfModel.getRedoCount()
        updateHistory(undoCount, redoCount)
        updateLSB(source)
    }

    onZoomPageFactChanged: {
        // preserve the position
        let pos = preservePosition()
        console.debug(JSON.stringify(pos))
        pdfModel.redrawAll()
        zoomFactorUpdate(zoomPageFact)
        jumpToPosition(pos)
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

        function updateCurrRect() {
            pdfPage.setCurrentNeedleRect(pdfModel.getCurrentNeedleRect(
                                             model.display))
            pdfPage.update()
            //console.warn("QML delegate updateCurrRect")
        }

        onWidthChanged: {
            if (root.zoomAuto) {
                pdfPage.width = width
            }
        }

        PdfPageRender {
            id: pdfPage

            property int aimResizeStatus: root.aimIsAlreadyResized
            property bool sizeKnown: false
            property int defaultWidth: root.pageWidth > 0
                                       && !sizeKnown ? root.pageWidth : root.width
            property int defaultHeight: root.pageHeight
                                        && !sizeKnown ? root.pageHeight : defaultWidth * 1.42

            customRotation: root.delegateRotation
            anchors.horizontalCenter: width < parent.width ? parent.horizontalCenter : undefined
            anchors.rightMargin: verticalScroll.width
            width: defaultWidth
            height: defaultHeight
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

            function updateTagCrossSize() {
                if (root.tagMode) {
                    let t_data = JSON.parse(tagData)
                    tagCross.width = t_data.tag_width * pdfPage.width / 100
                    tagCross.height = tagCross.width / root.ratio
                    console.warn("tag width = " + tagCross.width)
                    console.warn("tag height = " + tagCross.height)
                }
            }

            onWidthChanged: {
                root.pageWidth = width
                updateCrossSize()
                updateTagCrossSize()
                if (width > 0) {
                    lastPageWidth = width
                }
            }

            onZoomLastChanged: {
                // size is known after render
                sizeKnown = true
            }

            onHeightChanged: {
                root.pageHeight = height
                updateCrossSize()
                updateTagCrossSize()
                landscape = pdfPage.width > pdfPage.height
                if (height > 0) {
                    root.lastPageHeight = height
                }
            }

            onAimResizeStatusChanged: {
                updateCrossSize()
                updateTagCrossSize()
            }

            Component.onCompleted: {
                setCtx(pdfModel.getCtx())
                setDoc(pdfModel.getDoc())
                setPageNumber(model.display)
                // highlight the needles
                setNeedleHighlightRects(pdfModel.getNeedlesForPage(
                                            model.display))
                pdfPage.setRubberStamps(pdfModel.getRubberStampForPage(
                                            model.display))
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
                acceptedButtons: Qt.RightButton | Qt.LeftButton

                onEntered: {
                    cross.visible = root.signInProgress ? false : true
                    cursorShape = root.signInProgress ? Qt.BusyCursor : Qt.CrossCursor
                }
                onExited: {
                    cross.visible = false
                    cursorShape = Qt.ArrowCursor
                }
                onClicked: mouse => {
                               if ((mouse.button === Qt.RightButton)
                                   && root.signMode) {
                                   quitSignMode()
                                   mouse.accepted = true
                               }

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
                                   root.proceedSigning(location_data)
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

            MouseArea {
                id: rubberMouseArea

                enabled: root.tagMode
                anchors.fill: parent
                hoverEnabled: true
                cursorShape: Qt.CrossCursor
                acceptedButtons: Qt.RightButton | Qt.LeftButton

                onEntered: {
                    console.debug("enter height = " + tagCross.height)
                    let t_data = JSON.parse(tagData)
                    tagCross.width = t_data.tag_width * width / 100
                    console.debug("enter width" + tagCross.width)
                    if (root.ratio > 0) {
                        tagCross.height = tagCross.width / root.ratio
                    }
                    tagCross.visible = true
                    cursorShape = Qt.CrossCursor
                }

                onExited: {
                    tagCross.visible = false
                    cursorShape = Qt.ArrowCursor
                }

                onClicked: mouse => {
                               if ((mouse.button === Qt.RightButton)
                                   && root.tagMode) {
                                   disableTagMode()
                                   mouse.accepted = true
                               }
                           }

                onPressed: {
                    root.interactive = false
                    root.startX = mouseX
                    root.startY = mouseY
                }

                onPositionChanged: mouse => {
                                       if ((pressed && Math.abs(
                                                startX - mouseX) > 10)
                                           || root.tag_placing) {
                                           root.tag_placing = true
                                           if (mouseX > startX) {
                                               tagCross.x = startX
                                               tagCross.width = mouseX - startX
                                           } else {
                                               tagCross.x = mouseX
                                               tagCross.width = startX - mouseX
                                           }

                                           if (mouseY > startY) {
                                               tagCross.y = startY
                                               tagCross.height = tagCross.width / root.ratio
                                           } else {
                                               //tagCross.y = mouseY
                                               tagCross.height = tagCross.width / root.ratio
                                           }
                                           if (tagCross.x < 0
                                               || tagCross.x + tagCross.width > pdfPage.width
                                               || tagCross.y < 0
                                               || tagCross.y + tagCross.height > pdfPage.height) {
                                               tagCross.valid_position = false
                                           } else {
                                               tagCross.valid_position = true
                                           }
                                       } else {
                                           tagCross.x = mouseX
                                           tagCross.y = mouseY
                                           if (tagCross.x < 0
                                               || tagCross.x + tagCross.width > pdfPage.width
                                               || tagCross.y < 0
                                               || tagCross.y + tagCross.height > pdfPage.height) {
                                               tagCross.valid_position = false
                                           } else {
                                               tagCross.valid_position = true
                                           }
                                       }
                                   }

                onReleased: mouse => {
                                if ((mouse.button === Qt.RightButton)
                                    && root.tagMode) {
                                    disableTagMode()
                                    mouse.accepted = true
                                } else if (root.tagMode
                                           && tagCross.valid_position) {
                                    //console.warn("pdflistview tagdata = " + tagData)
                                    let t_data = JSON.parse(tagData)
                                    let rubber_stamp_data = {
                                        "page_index": index,
                                        "page_width": width,
                                        "page_height": height,
                                        "stamp_x": tagCross.x,
                                        "stamp_y": tagCross.y,
                                        "stamp_width": tagCross.width,
                                        "stamp_height": tagCross.height,
                                        "create_from_image": t_data.create_from_image,
                                        "img_path": t_data.img_path,
                                        "border_width": t_data.border_width,
                                        "border_radius": t_data.border_radius,
                                        "text_color_red": t_data.R,
                                        "text_color_green": t_data.G,
                                        "text_color_blue": t_data.B,
                                        "border_color_red": t_data.R,
                                        "border_color_green": t_data.G,
                                        "border_color_blue": t_data.B,
                                        "bg_color_red": t_data.R,
                                        "bg_color_green": t_data.G,
                                        "bg_color_blue": t_data.B,
                                        "font_family": t_data.font_family,
                                        "stamp_text": t_data.stamp_text,
                                        "bg_transparent": t_data.bg_transparent,
                                        "annotation_width": tagCross.width,
                                        "zoom_on_rubber_render": root.zoomPageFact,
                                        "link": t_data.stamp_link
                                    }

                                    root.size_estimated = false
                                    tagCross.visible = false
                                    cursorShape = Qt.BusyCursor
                                    root.tagMode = false
                                    console.debug("exit tag mode")
                                    root.interactive = true
                                    pdfModel.placeRubberStamp(rubber_stamp_data)
                                    root.tagInProgress = true
                                    root.forceActiveFocus()
                                    root.tag_placing = false
                                }
                            }

                Rectangle {
                    id: tagCross

                    property string defaultText: qsTr("Mark position")
                    property string invalidPositionText: qsTr(
                                                             "Invalid position")
                    property bool valid_position: true

                    color: "transparent"
                    border.color: valid_position ? "blue" : "red"
                    border.width: 2
                    visible: false

                    Text {
                        topPadding: 10
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: tagCross.valid_position ? tagCross.defaultText : tagCross.invalidPositionText
                        color: tagCross.valid_position ? "blue" : "red"
                        font.family: "Noto Sans"
                    }
                }
            }

            Connections {
                target: pdfModel

                function onSizeReady(calc_ratio) {
                    // add rubber stamps on render
                    root.ratio = calc_ratio
                    let t_data = JSON.parse(tagData)
                    tagCross.width = t_data.tag_width * pdfPage.width / 100
                    tagCross.height = tagCross.width / calc_ratio
                    root.size_estimated = true
                    console.debug("size ready height = " + tagCross.height)
                    console.debug("size ready width = " + tagCross.width)
                    console.debug("size ready = " + ratio)
                }

                function onUpdateDoc() {
                    // add rubber stamps on render
                    pdfPage.setRubberStamps(pdfModel.getRubberStampForPage(
                                                model.display))
                    tagPlaced()

                    let undoCount = pdfModel.getUndoCount()
                    let redoCount = pdfModel.getRedoCount()
                    updateHistory(undoCount, redoCount)
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
