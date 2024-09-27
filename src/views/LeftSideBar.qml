import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import alt.pdfcsp.pdfModel
import alt.pdfcsp.pdfRender

Item {
    id: root
    enum ShowState{
        Preview,
        Certs
    }

    property string source: ""
    property int showState : LeftSideBar.ShowState.Preview

    Layout.maximumWidth: 400
    Layout.preferredWidth:  200
    Layout.minimumWidth:  200
    Layout.fillHeight: true
    Layout.fillWidth: true
    Layout.horizontalStretchFactor: 1

    signal pageClick(int index)



    Item{
        anchors.fill: parent

        PreviewListView{
            id:previewListView
            source: root.source
            visible: showState == LeftSideBar.ShowState.Preview
        }

        SignaturesList{
            id: sigListView
            visible: showState == LeftSideBar.ShowState.Certs
        }

        // Item{
        //     id: certsInfo
        //     visible: showState == LeftSideBar.ShowState.Certs
        //     Layout.maximumWidth: 100
        //     Layout.fillHeight: true
        //     clip:true

        //     Rectangle{
        //         width:50
        //         height:50
        //         color:"white"
        //     }
        // }

    }


    function scrollToPage(newIndex) {
        previewListView.positionViewAtIndex(newIndex - 1, ListView.Beginning)
    }

    function showPreviews(){
        showState=LeftSideBar.ShowState.Preview
    }

    function showCerts(){
        showState=LeftSideBar.ShowState.Certs
    }
}
