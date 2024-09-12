import QtQuick
import QtQuick.Controls
import alt.pdfcsp.pdfModel
import alt.pdfcsp.pdfRender

ListView {
    id:root
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.fill: parent
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

    // delegate: PdfPage{
    //     text: model.display
    //     width: pdfListView.width-rightMargin-leftMargin
    //     height:500
    // }

    delegate: PdfPageRender{
        width:root.width;
        height: width*1.42;

        Component.onCompleted: {
            setCtx(pdfModel.getCtx())
            setDoc(pdfModel.getDoc())
            setPageNumber(model.display)
        }
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
