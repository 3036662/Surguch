import QtQuick

Column {
    id: root
    property string keyText
    property bool value

    Text{
        text:keyText
        wrapMode:Text.WordWrap
        maximumLineCount:3
    }
    Text{
        text:value ? qsTr("Valid") : qsTr("Invalid")
        color: value ? "grey" : "red"
    }

}
