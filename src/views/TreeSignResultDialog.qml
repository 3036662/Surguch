import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import StyleSheet

Dialog {
    id: root

    property var sign_result
    property bool sign_done: false

    title: qsTr("Signing result")
    modal: true
    width: 520
    height: 400
    padding: 12
    closePolicy: Popup.CloseOnEscape

    x: (parent.width - width) / 2
    y: (parent.height - height) / 2

    BusyIndicator {
        visible: !root.sign_done
        running: !root.sign_done
        anchors.centerIn: parent
        width: 64
        height: 64
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 10
        visible: root.sign_done

        Label {
            text: qsTr("Final directory")
            font.bold: true
            font.family: "Noto Sans"
            font.pixelSize: 12
            Layout.fillWidth: true
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: 8

            TextField {
                id: dirField
                readOnly: true
                text: sign_result?.final_dir ?? ""
                placeholderText: qsTr("No directory")
                font.family: "Noto Sans"
                font.pixelSize: 12
                Layout.fillWidth: true
            }

            Button {
                text: qsTr("Open")
                enabled: !!(sign_result?.final_dir)
                onClicked: Qt.openUrlExternally(
                               "file://" + sign_result.final_dir)
                           && root.close()
            }
        }

        Label {
            text: qsTr("Files")
            font.bold: true
            Layout.fillWidth: true
            font.family: "Noto Sans"
            font.pixelSize: 12
        }

        ScrollView {
            Layout.fillWidth: true
            Layout.maximumHeight: 220
            TextArea {
                id: filesArea
                Layout.fillWidth: true
                readOnly: true
                text: sign_result ? ((sign_result.files
                                      && sign_result.files.length
                                      > 0) ? sign_result.files.join(
                                                 "\n") : sign_result.warnings.join(
                                                 "\n")) : ""

                placeholderText: qsTr("No files")
                selectByMouse: true
                wrapMode: Text.WordWrap
                placeholderTextColor: "grey"
                font.family: "Noto Sans"
                font.pixelSize: 12
                color: StyleSheet.font_color_extra

                background: Rectangle {
                    border.color: StyleSheet.slider_border_color
                    color: StyleSheet.text_area_background
                }
            }
            ScrollBar.vertical: ScrollBar {
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.rightMargin: 10
                width: 8
                policy: (filesArea.lineCount > 10) ? ScrollBar.AlwaysOn : ScrollBar.AlwaysOff

                contentItem: Rectangle {
                    implicitWidth: 6
                    radius: width / 2
                    color: StyleSheet.slider_fill_color
                    border.width: 2
                    border.color: StyleSheet.slider_border_color
                }

                background: Rectangle {
                    color: "transparent"
                }
            }
        }

        DialogButtonBox {
            Layout.alignment: Qt.AlignRight
            standardButtons: Dialog.Ok
            onAccepted: root.close()
        }
    }
}
