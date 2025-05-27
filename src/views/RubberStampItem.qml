import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import StyleSheet

Item {
    id: root

    property var rubber_model

    RowLayout {
        width: root.width

        RadioButton {
            id: radioSelect
            text: title
            Layout.preferredHeight: 30
            Layout.fillWidth: true
            ButtonGroup.group: rubberStampGroup

            onClicked: {
                rubberStampEditor.edit_state = false
                //set a reference to model
                rubberStampEditor.rubber_model = rubber_model
                //if creating new stamp, set an empty data
                if (value === "new") {
                    rubberStampEditor.stamp_data = ""
                    rubberStampEditor.stamp_id = -1
                    rubberStampEditor.updateRubberStampForm()
                    rubberStampEditor.open()
                }
            }

            Component.onCompleted: {
                if (value === "new") {
                    indicator.width = 0
                    indicator.height = 0
                }
            }
        }

        // Rectangle {
        //     Layout.fillWidth: true
        // }

        ToolButton {
            id: settingsButton
            visible: value !== "new"

            icon.source: StyleSheet.wrench_icon
            icon.width: 20
            icon.height: 20

            onClicked: {
                rubberStampEditor.edit_state = true
                //set a reference to model
                rubberStampEditor.rubber_model = rubber_model
                //set data data about selected stamp
                rubberStampEditor.stamp_data = value
                //update data in setting window
                rubberStampEditor.updateRubberStampForm()
                rubberStampEditor.open()
            }
        }
    }
}
