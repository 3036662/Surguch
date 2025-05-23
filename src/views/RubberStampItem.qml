import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import StyleSheet

Item {
    id: root

    RowLayout {
        width: root.width

        RadioButton {
            id: radioSelect
            text: title
            Layout.preferredHeight: 30
            ButtonGroup.group: rubberStampGroup

            onClicked: {
                rubberStampEditor.editState = false
                rubberStampEditor.open()
            }
            Component.onCompleted: {
                if (value === "new") {
                    indicator.width = 0
                    indicator.height = 0
                }
            }
        }

        Rectangle {
            Layout.fillWidth: true
        }

        ToolButton {
            id: settingsButton
            visible: value !== "new"

            icon.source: StyleSheet.wrench_icon
            icon.width: 20
            icon.height: 20
            highlighted: false

            onClicked: {
                console.warn("open me settings")
            }
        }
    }
}
