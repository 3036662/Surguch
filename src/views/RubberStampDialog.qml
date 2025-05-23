import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import StyleSheet

Dialog {
    id: root

    width: 300
    height: 300
    x: rubberStampDialogButton.x - width / 2
    y: parent.y

    ButtonGroup {
        id: rubberStampGroup
    }

    ListView {
        width: root.width
        anchors.fill: parent
        model: rubberStampModel
        delegate: ItemDelegate {
            height: 30
            width: parent.width
            RubberStampItem {
                height: parent.height
                width: parent.width
            }
        }

    }

}
