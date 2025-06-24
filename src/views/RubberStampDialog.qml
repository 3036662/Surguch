import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import StyleSheet

Dialog {
    id: root

    width: 300
    height: Math.min(300, itemView.contentHeight + 20)
    x: rubberStampDialogButton.x - width / 2
    y: parent.y
    clip: true

    ButtonGroup {
        id: rubberStampGroup
    }

    ListView {
        id: itemView

        anchors.fill: parent
        model: rubberStampModel

        ScrollBar.vertical: ScrollBar {
        }

        delegate: ItemDelegate {
            height: 35
            width: 230
            RubberStampItem {
                height: parent.height
                width: parent.width
                rubber_model: itemView.model
            }
        }

    }

}
