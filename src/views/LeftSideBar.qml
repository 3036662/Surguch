import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import alt.pdfcsp.pdfModel
import alt.pdfcsp.pdfRender

Item {
    id: root

    enum ShowState {
        Preview,
        Certs
    }
    property string source: ""
    property int showState: LeftSideBar.ShowState.Preview
    property int sigCount: 0

    signal pageClick(int index)
    signal showSigData(string data)

    function scrollToPage(newIndex) {
        previewListView.positionViewAtIndex(newIndex - 1, ListView.Beginning)
    }

    function showPreviews() {
        console.warn("show Previews func")
        showState = LeftSideBar.ShowState.Preview
    }

    function showCerts() {
        console.warn("showCerts function")
        showState = LeftSideBar.ShowState.Certs
    }

    function setSigCount(sig_count) {
        sigCount = sig_count

    }

    Layout.maximumWidth: 300
    Layout.preferredWidth: 300
    Layout.minimumWidth: 200
    Layout.fillHeight: true
    Layout.fillWidth: true
   // Layout.horizontalStretchFactor: 1

    Item {
        anchors.fill: parent

        PreviewListView {
            id: previewListView
            source: root.source
            visible: showState == LeftSideBar.ShowState.Preview

        }

        SignaturesList {
            id: sigListView
            visible: showState == LeftSideBar.ShowState.Certs
                     && sigCount > 0
        }

        Item {
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.fill: parent
            visible: showState == LeftSideBar.ShowState.Certs
                     && sigCount === 0
            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: parent.top
                topPadding: 20
                text: qsTr("No signatures")
                font.family: "Noto Sans"
            }
        }
    }
}
