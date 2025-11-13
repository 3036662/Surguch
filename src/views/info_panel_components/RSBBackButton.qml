import QtQuick
import QtQuick.Controls
import StyleSheet

ToolButton {
    flat: true
    display: AbstractButton.IconOnly
    icon.source: StyleSheet.back_icon
    icon.width: 20
    icon.height: 20
    icon.color: StyleSheet.font_color
    anchors.left: parent.left
    anchors.top: parent.top
    leftPadding: 0
    topPadding: 0
    rightPadding: 0
    bottomPadding: 0
    width: 20
    height: 20
}
