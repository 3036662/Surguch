import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root
    property alias edit_profile: edit_profile_panel

    enum ShowState {
        Invisible,
        SigInfo,
        ProfileInfo
    }

    property int showState: RightSideBar.ShowState.Invisible
    property var jsonData

    visible: showState != RightSideBar.ShowState.Invisible
    Layout.maximumWidth: 400
    Layout.preferredWidth: 300
    Layout.minimumWidth: 200
    Layout.fillHeight: true
    Layout.fillWidth: true
    Layout.horizontalStretchFactor: 1

    function showData(data) {
        try {
            //console.warn(data)
            if (!data) {
                jsonData = undefined
                return
            }
            jsonData = JSON.parse(data)
            showState = RightSideBar.ShowState.SigInfo
        } catch (e) {
            console.error("Error parsing JSON" + e.message)
        }
    }

    Rectangle {
        anchors.fill: parent
        color: "white"
    }

    RSBSigInfo {
        visible: showState == RightSideBar.ShowState.SigInfo
    }

    EditProfile {
        id: edit_profile_panel
        visible: showState == RightSideBar.ShowState.ProfileInfo
    }
}
