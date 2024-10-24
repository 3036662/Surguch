import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import alt.pdfcsp.pdfModel

ApplicationWindow {
    id: root_window
    width: 1000
    height: 480
    visible: true
    title: qsTr("PDF CSP")

    header: ToolBar {
        id: toolbar
        topPadding: 2
        ColumnLayout {
            anchors.fill: parent
            spacing: 2
            Header {
                id:header
            }
            HeaderSubBar {
                id: headerSubBar
                visible: pdfListView.source != ""
            }
        }
    }

    RowLayout {
        anchors.fill: parent

        LeftSideBar {
            id: leftSideBar
        }
        PdfListView {
            id: pdfListView
        }

        RightSideBar {
            id: rightSideBar
        }

    }

    footer: Pane {
        id: footer_frame
        width: parent.width
        height: 30
        ScrollBar {
            id: horizontalScroll
            hoverEnabled: true
            active: hovered || pressed
            orientation: Qt.Horizontal
            size: contWidth > 0 ? pdfListView.width / contWidth : 0
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            visible: contWidth > pdfListView.width
            width: pdfListView.width
            property int contWidth: 0

            // move the listview when the scroll was moved
            onPositionChanged: {
                pdfListView.contentX = position * contWidth
                pdfListView.hScrollPos = position
                console.warn(position + size)
            }

            // page width changed
            function setContentWidth(w) {
                size = pdfListView.width / (w > 0 ? w : 1)
                contWidth = w
            }

            // listview flicked
            function updateScrollPosition(newPosition) {
                if (visible) {
                    position = newPosition / contWidth
                }
            }
        }
    }

    MuPdfModel {
        id: pdfModel
        mustProcessSignatures: true
    }


    Component.onCompleted: {
        // update page count in header
        pdfListView.pagesCountChanged.connect(headerSubBar.changePageCount)
        // update curr page in header
        pdfListView.currPageChanged.connect(headerSubBar.changedCurrPage)
        pdfListView.currPageChanged.connect(leftSideBar.scrollToPage)
        pdfListView.pageWidthUpdate.connect(horizontalScroll.setContentWidth)
        // scroll to page
        headerSubBar.scrollToPage.connect(pdfListView.scrollToPage)
        leftSideBar.pageClick.connect(pdfListView.scrollToPage)
        // show signature info
        leftSideBar.showSigData.connect(rightSideBar.showData)
        // update zoom value in header
        pdfListView.zoomFactorUpdate.connect(headerSubBar.updateZoomValue)
        // update horizontal scroll position after flick
        pdfListView.hScrollUpdate.connect(horizontalScroll.updateScrollPosition)
        // zoomIn render
        headerSubBar.zoomInClicked.connect(pdfListView.zoomIn)
        // zoomOut render
        headerSubBar.zoomOutClicked.connect(pdfListView.zoomOut)
        // zoom dropbox preset value was selected
        headerSubBar.zoomSelected.connect(pdfListView.setZoom)
        // rotate
        headerSubBar.rotateClockwise.connect(pdfListView.rotateClockWise)
        headerSubBar.rotateCounterClockWise.connect(
                    pdfListView.rotateCounterClockWise)
        // enable/disable zoom
        pdfListView.maxZoomReached.connect(headerSubBar.disableZoom)
        pdfListView.canZoom.connect(headerSubBar.enableZoom)
        pdfListView.minZoomReached.connect(headerSubBar.disableZoomOut)
        pdfListView.canZoomOut.connect(headerSubBar.enableZoomOut)
        // toggle from preview to certs in left sidebat
        headerSubBar.showPreviews.connect(leftSideBar.showPreviews)
        headerSubBar.showCerts.connect(leftSideBar.showCerts)
    }

    // Info dialog in center of window
    Dialog {
        id: infoDialog
        width: root_window.width - 200
        height: root_window.height - 100
        modal: true
        standardButtons: Dialog.Ok
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2

        Loader {
            id: infoDialogContentContainer
            width: parent.width
            height: parent.height
        }

        // Handle dialog closing
        onAccepted: {
            console.log("Dialog closed")
        }
    }
}
