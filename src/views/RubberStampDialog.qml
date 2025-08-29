import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import StyleSheet

Popup {
    id: root

    width: 300
    height: Math.min(300, itemView.contentHeight + 20)
    clip: true

    x: headerSubBar.rubberStampDialogButtonX - width
    y: StyleSheet.defaultPaddingV

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
            RubberStampListEntry {
                anchors.fill: parent
                anchors.margins: 1
                rubber_model: itemView.model
            }
        }
    }
}
