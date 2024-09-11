import QtQuick
import QtQuick.Controls
import alt.pdfcsp.pdfModel

ListView {
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.fill: parent
    leftMargin: 30
    rightMargin: 30
    spacing: 30

    signal pagesCountChanged(int count)
    signal currPageChanged(int index)

    property string source: ""
    onSourceChanged:{
        pdfModel.setSource(source)
    }

    model: MuPdfModel{
        id: pdfModel
    }

    delegate: PdfPage{
        text: model.display
        width: pdfListView.width-rightMargin-leftMargin
        height:500
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
        console.warn("currentIndex")
    }
}
