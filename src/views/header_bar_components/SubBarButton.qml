import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import StyleSheet

ToolButton {
    flat: true
    display: AbstractButton.IconOnly
    icon.width: 20
    icon.height: 20
    implicitHeight: 30
    leftPadding: 5
    rightPadding: 5

    ToolTip {
        text: parent.text
        visible: parent.hovered
        delay: 500
    }
}
