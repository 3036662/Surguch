import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ApplicationWindow {
    id: root
    width: 1000
    height: 480
    visible: true
    title: qsTr("Hello World")

    header: ToolBar {
        id: toolbar
        topPadding: 2
        ColumnLayout {
            anchors.fill: parent
            spacing: 2
            Header {}
            HeaderSubBar {
                id: headerSubBar
            }
        }
    }

    PdfListView {
        id: pdfListView
    }

    Component.onCompleted: {
        pdfListView.pagesCountChanged.connect(headerSubBar.changePageCount)
        pdfListView.currPageChanged.connect(headerSubBar.changedCurrPage)
        pdfListView.pageWidthUpdate.connect(horizontalScroll.setContentWidth)
        pdfListView.pageWidthUpdate.connect(horizontalScroll.setContentWidth)
        pdfListView.flickEnded.connect(horizontalScroll.setPosition)
        headerSubBar.zoomInClicked.connect(pdfListView.zoomIn)
        headerSubBar.zoomOutClicked.connect(pdfListView.zoomOut)
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
            size: contWidth > 0 ? footer_frame.width / contWidth : 0
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            visible: contWidth > width
            property int contWidth: 0

            onPositionChanged: {
                pdfListView.contentX = position * contWidth
                pdfListView.hScrollPos = position
            }

            // called when page width was changed
            function setContentWidth(w) {
                size = footer_frame.width / (w > 0 ? w : 1)
                contWidth = w
            }
        }
    }
}
