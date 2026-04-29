import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import alt.pdfcsp.pdfModel
import alt.pdfcsp.pdfRender
import StyleSheet

import "info_panel_components" as InfoPanelComponents

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
        showState = LeftSideBar.ShowState.Preview
    }

    function showCerts() {
        showState = LeftSideBar.ShowState.Certs
    }

    function setSigCount(sig_count) {
        sigCount = sig_count
    }

    function updateSource(new_src) {
        source = new_src
    }

    Layout.maximumWidth: 200
    Layout.preferredWidth: 200
    Layout.minimumWidth: 200
    Layout.fillHeight: true
    Layout.fillWidth: true

    Item {
        anchors.fill: parent

        PreviewListView {
            id: previewListView
            source: root.source
            visible: showState == LeftSideBar.ShowState.Preview
        }

        InfoPanelComponents.SignaturesList {
            id: sigListView
            visible: showState == LeftSideBar.ShowState.Certs && sigCount > 0
        }

        Item {
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.fill: parent
            visible: showState == LeftSideBar.ShowState.Certs && sigCount === 0
            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: parent.top
                topPadding: 20
                text: qsTr("No signatures")
                font.family: StyleSheet.defaultFontFamily
                font.pointSize: StyleSheet.defaultTextPointSize
                color: StyleSheet.font_color_extra
            }
        }
    }
}
