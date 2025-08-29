import QtQuick
import QtQuick.Controls

Column {
    id: root

    property string title
    property var items
    property bool status

    RightSBHorizontalDelimiter {
        width: parent.width
    }

    TextPairBool {
        keyText: qsTr("Certificate chain")
        value: root.status
    }

    Column {
        anchors.left: parent.left
        anchors.leftMargin: 20

        Repeater {
            model: root.items

            MouseArea {
                id: cert_clickable_area
                required property var modelData
                width: childrenRect.width
                height: childrenRect.height
                onClicked: {
                    infoDialogContentContainer.source = "CertInfo.qml"
                    if (infoDialogContentContainer.item) {
                        infoDialogContentContainer.item.cert = cert_clickable_area.modelData
                    }
                    infoDialog.open()
                }

                TextPairBool {

                    keyText: modelData.subject !== undefined ? modelData.subject_common_name : ""
                    value: modelData.trust_status !== undefined ? modelData.trust_status : ""
                }
            }
        }
    }
    RightSBHorizontalDelimiter {
        width: parent.width
    }
}
