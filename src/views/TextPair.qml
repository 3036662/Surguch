import QtQuick

Column {
    property string keyText
    property string value

    Text{
        text:keyText
    }
    Text{
        text:value
        color: "grey"
    }

}
