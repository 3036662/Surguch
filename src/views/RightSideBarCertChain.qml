import QtQuick

Column {
    id: root
    property string title
    property var items


    RightSBHorizontalDelimiter {
        width: parent.width
    }

    TextPairBool {
        keyText: qsTr("Certificate chain")
        value: false
    }

    Column {
        anchors.left: parent.left
        anchors.leftMargin: 20

        Repeater {
            model: root.items

            TextPairBool {
                required property var modelData
                keyText: modelData.key
                value: modelData.value
            }
        }
    }
    RightSBHorizontalDelimiter {
        width: parent.width
    }
}
