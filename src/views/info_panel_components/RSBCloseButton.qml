import QtQuick
import QtQuick.Controls
import StyleSheet

ToolButton {
    flat: true
    display: AbstractButton.IconOnly
    icon.source: StyleSheet.close_icon
    icon.width: 20
    icon.height: 20
    icon.color: StyleSheet.font_color
    leftPadding: 0
    topPadding: 0
    rightPadding: 0
    bottomPadding: 0
    anchors.top: parent.top
    anchors.right: parent.right
    width: 20
    height: 20
}
