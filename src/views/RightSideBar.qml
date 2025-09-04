import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import StyleSheet

import "info_panel_components" as InfoPanelComponents

Item {
    id: root

    signal profileSaved

    enum ShowState {
        Invisible,
        SigInfo,
        ProfileInfo,
        Certs,
        Mrpa
    }

    property alias edit_profile: edit_profile_panel
    property int showState: RightSideBar.ShowState.Invisible
    property var jsonData
    property int sigCount: 0

    function showData(data) {
        try {
            if (!data) {
                jsonData = undefined
                return
            }
            //console.warn(data);
            jsonData = JSON.parse(data)
            showState = RightSideBar.ShowState.SigInfo
            sigInfoPanel.contentY = 0
        } catch (e) {
            console.error("Error parsing JSON" + e.message)
        }
    }

    visible: showState != RightSideBar.ShowState.Invisible
    Layout.maximumWidth: 300
    Layout.preferredWidth: 300
    Layout.minimumWidth: 300
    Layout.fillHeight: true
    Layout.fillWidth: true

    //Layout.horizontalStretchFactor: 1
    Rectangle {
        anchors.fill: parent
        //border.color: "#454A4E"
        color: StyleSheet.edit_background
    }

    InfoPanelComponents.RSBSigInfo {
        id: sigInfoPanel
        visible: showState == RightSideBar.ShowState.SigInfo

        onCloseClicked: {
            showState = RightSideBar.ShowState.Invisible
        }
    }

    EditProfile {
        id: edit_profile_panel
        visible: showState == RightSideBar.ShowState.ProfileInfo

        onProfileSaved: {
            root.profileSaved()
        }
        onCloseClicked: {
            showState = RightSideBar.ShowState.Invisible
        }
    }

    InfoPanelComponents.SignaturesList {
        id: rSigListView
        visible: showState == LeftSideBar.ShowState.Certs && sigCount > 0
    }
}
