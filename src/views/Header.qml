import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt.labs.platform

RowLayout {
    id: toolbar_layout
    spacing: 5

    Row {
        Layout.minimumWidth: 600
        Layout.fillWidth: true
        TopBarButton {
            icon.source: "qrc:/icons/file_plus.svg"
            text: qsTr("Open")
            onClicked: fileDialog.open()
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
            height: parent.height
            Rectangle {
                width: 5
                height: parent.height
                color: "transparent"
            }
            Image {
                Layout.alignment: Qt.AlignVCenter
                id: wrench_icon
                source: "qrc:/icons/wrench.svg"
                height: parent.height
                fillMode: Image.PreserveAspectFit
            }
            Rectangle {
                width: 5
                height: parent.height
                color: "transparent"
            }
            ComboBox {
                Layout.alignment: Qt.AlignVCenter
                id: comboBox
                model: ["Option 1", "Option 2", "Option 3", "Option 4"]
                displayText: qsTr("Profile")
                implicitContentWidthPolicy: ComboBox.ContentItemImplicitWidth
                anchors.verticalCenter: parent.verticalCenter
            }
            Rectangle {
                width: 5
                height: parent.height
                color: "transparent"
            }
        }
        TopBarButton {
            icon.source: "qrc:/icons/pencil_line.svg"
            text: qsTr("Sign")
            icon.height: 25
            icon.width: 25
            anchors.top: parent.top
            anchors.bottom: parent.bottom
        }
    }

    ToolButton {
        flat: true
        display: AbstractButton.IconOnly
        icon.source: "qrc:/icons/bell.svg"
        icon.width: 30
        icon.height: 30
        leftPadding: 10
        rightPadding: 10
    }

    FileDialog {
        id: fileDialog
        currentFile: ""
        fileMode: FileDialog.OpenFile
        nameFilters: ["Pdf files (*.pdf)"]
        folder: StandardPaths.writableLocation(StandardPaths.DocumentsLocation)
        onAccepted: {
            pdfListView.source = currentFile
            leftSideBar.source = currentFile
            rightSideBar.showState = RightSideBar.ShowState.Invisible
        }
    }
}
