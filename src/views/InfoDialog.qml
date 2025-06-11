import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import StyleSheet

Dialog {
    id: root

    width: 400
    height: 400
    leftMargin: 10
    rightMargin: 10
    modal: true
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2
}