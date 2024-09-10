import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ApplicationWindow {
    id: root
    width: 800
    height: 480
    visible: true
    title: qsTr("Hello World")

    header: ToolBar {
        id: toolbar

        RowLayout {
            id: toolbar_layout
            anchors.fill: parent
            spacing: 5

            RowLayout {
                Layout.minimumWidth: 600
                Layout.alignment: Qt.AlignLeft
                TopBarButton {
                    icon.source: "qrc:/icons/file_plus.svg"
                    text: qsTr("Open")
                }
                TopBarButton {
                    icon.source: "qrc:/icons/file_simple.svg"
                    text: qsTr("Show in folder")
                }
                TopBarButton {
                    icon.source: "qrc:/icons/folder_plus.svg"
                    text: qsTr("Save as ...")
                }
                TopBarButton {
                    icon.source: "qrc:/icons/printer_sm.svg"
                    text: qsTr("Print")
                }
                Row {
                    spacing: 2
                    id: comboBox_row
                    leftPadding: 10


                    Image{
                        id:wrench_icon
                        source:"qrc:/icons/wrench.svg"
                        height: 30
                        fillMode: Image.PreserveAspectFit
                    }
                    Rectangle{
                       width: 5
                       height: 30
                    color: "transparent"
                    }


                    ComboBox {
                        id: comboBox
                        model: ["Option 1", "Option 2", "Option 3", "Option 4"]
                        displayText: qsTr("Profile")
                        implicitContentWidthPolicy: ComboBox.ContentItemImplicitWidth
                    }
                }
                TopBarButton {
                    icon.source: "qrc:/icons/pencil_line.svg"
                    text: qsTr("Sign")
                    icon.height: 30
                    icon.width:30

                }
            }

            ToolButton{
                flat:true
                display:AbstractButton.IconOnly
                icon.source: "qrc:/icons/bell.svg"
                icon.width: 30
                icon.height: 30
                leftPadding: 10
                rightPadding: 3
                Layout.alignment: Qt.AlignRight
            }
        }
    }

    background: Rectangle {
        color: "darkGrey"
    }
}
