import QtQuick

Column {
    property string keyText
    property bool value

    Text{
        text:keyText
    }
    Text{
        text:value ? qsTr("Valid") : qsTr("Invalid")
        color: value ? "grey" : "red"
    }

}
