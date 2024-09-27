import QtQuick
import alt.pdfcsp.signaturesListModel

ListView{
    id:root
    anchors.leftMargin: 5
    anchors.rightMargin: 5
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.fill: parent
    spacing: 5
    flickableDirection: Flickable.VerticalFlick

    model:SignaturesListModel{ id:siglistModel}

    delegate: Row{
        width: 50
        height:50
        Rectangle{
            width:root.width
            height:30
            color:"white"
            border.color: "black"
            border.width:2

            Text{
                text:model.sigInfo
            }
        }
    }

    Component.onCompleted: {
       pdfModel.signaturesFound.connect(siglistModel.updateSigList);
    }
}
