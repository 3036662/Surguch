import QtQuick

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

            TextPairBool {
                required property var modelData
                keyText: modelData.subject!==undefined ? modelData.subject_common_name : "";
                value: modelData.trust_status!==undefined ? modelData.trust_status :"";
            }
        }
    }
    RightSBHorizontalDelimiter {
        width: parent.width
    }
}
