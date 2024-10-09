import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import alt.pdfcsp.pdfModel
import alt.pdfcsp.pdfRender
import alt.pdfcsp.signaturesListModel

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
    signal showSigData(string data)



    Item{
        anchors.fill: parent

        PreviewListView{
            id:previewListView
            source: root.source
            visible: showState == LeftSideBar.ShowState.Preview
        }

        SignaturesListModel {
            id: siglistModel
        }

        SignaturesList{
            id: sigListView
            visible: showState == LeftSideBar.ShowState.Certs && siglistModel.rowCount()>0
        }

        Item{
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.fill:parent
            visible: showState == LeftSideBar.ShowState.Certs && siglistModel.rowCount()===0
            Text{
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: parent.top
                topPadding: 20
                text: qsTr("No signatures")
            }
        }

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
