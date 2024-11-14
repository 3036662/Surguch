import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root

    enum ShowState {
        Invisible,
        SigInfo,
        ProfileInfo
    }

    property alias edit_profile: edit_profile_panel
    property int showState: RightSideBar.ShowState.Invisible
    property var jsonData

    function showData(data) {
        try {
            if (!data) {
                jsonData = undefined
                return
            }
            console.warn(data);
            jsonData = JSON.parse(data)
            showState = RightSideBar.ShowState.SigInfo
            sigInfoPanel.contentY=10;

        } catch (e) {
            console.error("Error parsing JSON" + e.message)
        }
    }

    visible: showState != RightSideBar.ShowState.Invisible
    Layout.maximumWidth: 400
    Layout.preferredWidth: 300
    Layout.minimumWidth: 200
    Layout.fillHeight: true
    Layout.fillWidth: true
    Layout.horizontalStretchFactor: 1

    Rectangle {
        anchors.fill: parent
        color: "white"
    }

    RSBSigInfo {
        id: sigInfoPanel
        visible: showState == RightSideBar.ShowState.SigInfo
    }

    EditProfile {
        id: edit_profile_panel
        visible: showState == RightSideBar.ShowState.ProfileInfo
    }
}
