import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Pdf

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
            HeaderSubBar {}
        }
    }


    PdfDocument {
               id: doc
               source: ""
    }

    PdfMultiPageView{
        id: pdf_view
        anchors.fill: parent
        document: doc

    }

    // background: Rectangle {
    //     color: "darkGrey"
    // }
}
