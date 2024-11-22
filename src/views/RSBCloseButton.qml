import QtQuick
import QtQuick.Controls

ToolButton {
    flat: true
    display: AbstractButton.IconOnly
    icon.source: "qrc:/icons/close_icon.svg"
    icon.width: 20
    icon.height: 20
    icon.color: "black"
    leftPadding: 0
    topPadding: 0
    rightPadding: 0
    bottomPadding: 0
    anchors.top: parent.top
    anchors.right: parent.right
    width:20
    height:20

    onClicked: {
        rightSideBar.showState = RightSideBar.ShowState.Invisible
    }
}
