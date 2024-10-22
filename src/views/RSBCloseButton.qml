import QtQuick
import QtQuick.Controls

ToolButton {
    flat: true
    display: AbstractButton.IconOnly
    icon.source: "qrc:/icons/close_icon.svg"
    icon.width: 20
    icon.height: 20
    leftPadding: 5
    topPadding: 30
    rightPadding: 30
    anchors.top: parent.top
    anchors.right: parent.right

    onClicked: {
        rightSideBar.showState = RightSideBar.ShowState.Invisible
    }
}
