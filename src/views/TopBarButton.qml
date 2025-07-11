import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import StyleSheet


ToolButton {
    flat: true
    display: StyleSheet.window_size_x === "normal" ? AbstractButton.TextBesideIcon : AbstractButton.IconOnly
    icon.width: 30
    icon.height: 30
    leftPadding: 10
    rightPadding: 10
    topPadding: 10
    bottomPadding: 10
    font.family: "Noto Sans"

    ToolTip {
        id: buttonTooltip
        text: parent.text
        visible: {
            return StyleSheet.window_size_x === "small_width" && parent.hovered && parent.text !== ""
        }
        delay: 100
    }
}
