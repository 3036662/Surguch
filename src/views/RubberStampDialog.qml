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
        spacing: 1
        clip: true
        rightMargin: scrollBar.width + 2

        ScrollBar.vertical: ScrollBar {
            id: scrollBar
            padding: 1
            width: 8
            policy: ScrollBar.AlwaysOn
        }

        delegate: ItemDelegate {
            height: 35
            width: itemView.width - itemView.rightMargin
            RubberStampItem {
                anchors.fill: parent
                anchors.margins: 1
                rubber_model: itemView.model
            }
        }
    }
}
