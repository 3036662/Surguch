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
        Mrpa,
        MrpaInfo
    }

    property alias edit_profile: edit_profile_panel
    property int showState: RightSideBar.ShowState.Invisible
    property var jsonData
    property int sigCount
    property var mrpaListData
    property var mrpaData

    signal showSigData(string data)
    signal showMrpaData(var data)

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

    function showMrpaList(data) {
        try {
            if (!data) {
                mrpaListData = undefined
                return
            }
            mrpaListData = data
            showState = RightSideBar.ShowState.Mrpa
            rMrpaListView.contentY = 0
        } catch (e) {
            console.error("Error parsing JSON" + e.message)
        }
    }

    function showMrpa(data) {
        try {
            if (!data) {
                mrpaData = undefined
                return
            }
            mrpaData = data
            showState = RightSideBar.ShowState.MrpaInfo
        } catch (e) {
            console.error("Error parsing JSON" + e.message)
        }
    }

    function setSigCount(sig_count) {
        sigCount = sig_count
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

        onBackClicked: {
            rightSideBar.showState = RightSideBar.ShowState.Certs
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
        visible: showState == RightSideBar.ShowState.Certs && sigCount > 0

        onCloseClicked: {
            showState = RightSideBar.ShowState.Invisible
        }
    }

    InfoPanelComponents.MrpaInfo {
        id: mrpaInfoPanel
        visible: showState == RightSideBar.ShowState.MrpaInfo

        onCloseClicked: {
            showState = RightSideBar.ShowState.Invisible
        }

        onBackClicked: {
            rightSideBar.showState = RightSideBar.ShowState.Mrpa
        }
    }

    InfoPanelComponents.MrpaList {
        id: rMrpaListView
        visible: showState == RightSideBar.ShowState.Mrpa

        onCloseClicked: {
            showState = RightSideBar.ShowState.Invisible
        }
    }
}
