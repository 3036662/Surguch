import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ColumnLayout {
    spacing: 1
    Rectangle {
        height: 1
        color: "grey"
        Layout.fillWidth: true
    }
    RowLayout {
        id: toolbar_subpanel
        spacing: 5
        ToolButton {
            flat: true
            display: AbstractButton.IconOnly
            icon.source: "qrc:/icons/book.svg"
            icon.width: 20
            icon.height: 20
            leftPadding: 40
            rightPadding: 40
        }

        HeaderToolSeparator {}

        ToolButton {
            flat: true
            display: AbstractButton.IconOnly
            icon.source: "qrc:/icons/pen_tool.svg"
            icon.width: 20
            icon.height: 20
            leftPadding: 40
            rightPadding: 40
        }

        HeaderToolSeparator {}

        ToolButton {
            flat: true
            display: AbstractButton.IconOnly
            icon.source: "qrc:/icons/arrow-circle-down.svg"
            icon.width: 20
            icon.height: 20
            leftPadding: 5
            rightPadding: 5
        }

        ToolButton {
            flat: true
            display: AbstractButton.IconOnly
            icon.source: "qrc:/icons/arrow-circle-up.svg"
            icon.width: 20
            icon.height: 20
            leftPadding: 5
            rightPadding: 5
        }

        TextField {
            text: "1"
            maximumLength: 100
            validator: IntValidator {
                bottom: 1
                top: 10000
            }
            Layout.preferredWidth: 30
            horizontalAlignment: TextInput.AlignHCenter
        }

        Text {
            text: "1" + qsTr(" of ") + "10"
            leftPadding: 10
            rightPadding: 10
        }

        HeaderToolSeparator {}

        ToolButton {
            flat: true
            display: AbstractButton.IconOnly
            icon.source: "qrc:/icons/arrow_back_curve.svg"
            icon.width: 20
            icon.height: 20
            leftPadding: 5
            rightPadding: 5
        }

        ToolButton {
            flat: true
            display: AbstractButton.IconOnly
            icon.source: "qrc:/icons/arrow_forward_curve.svg"
            icon.width: 20
            icon.height: 20
            leftPadding: 5
            rightPadding: 5
        }

        HeaderToolSeparator {}

        ToolButton {
            flat: true
            display: AbstractButton.IconOnly
            icon.source: "qrc:/icons/minus-circle.svg"
            icon.width: 20
            icon.height: 20
            leftPadding: 5
            rightPadding: 5
        }

        ToolButton {
            flat: true
            display: AbstractButton.IconOnly
            icon.source: "qrc:/icons/plus-circle.svg"
            icon.width: 20
            icon.height: 20
            leftPadding: 5
            rightPadding: 5
        }
        Row {
            leftPadding: 10
            rightPadding: 10
            ComboBox {
                Layout.alignment: Qt.AlignVCenter
                id: comboBox
                model: ["Option 1", "Option 2", "Option 3", "Option 4"]
                displayText: qsTr("Automatic")
                implicitContentWidthPolicy: ComboBox.ContentItemImplicitWidth
                anchors.verticalCenter: parent.verticalCenter
            }
        }

        HeaderToolSeparator {}

        Row {
            leftPadding: 10

            TextField {
                placeholderText: qsTr("Search")
                maximumLength: 100
                Layout.preferredWidth: 150
                horizontalAlignment: TextInput.AlignLeft
                rightPadding: 30
                Button {
                    flat: true
                    icon.source: "qrc:/icons/search-custom.svg"
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    height: 24
                    smooth: true
                    anchors.right: parent.right
                }
            }
        }
        Rectangle {
            color: "transparent"
            Layout.fillHeight: true
            Layout.fillWidth: true
        }

        ToolButton {
            flat: true
            display: AbstractButton.IconOnly
            icon.source: "qrc:/icons/fullscreen-custom.svg"
            icon.width: 20
            icon.height: 20
            leftPadding: 5
            rightPadding: 5
            Layout.alignment: Qt.AlignRight
        }
    }
}
