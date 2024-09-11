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
            HeaderSubBar {id:headerSubBar}
        }
    }

    PdfListView{ id:pdfListView }


    Component.onCompleted: {
          pdfListView.pagesCountChanged.connect(headerSubBar.changePageCount);
          pdfListView.currPageChanged.connect(headerSubBar.changedCurrPage);
    }


    // background: Rectangle {
    //     color: "darkGrey"
    // }
}
