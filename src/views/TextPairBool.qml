import QtQuick
import QtQuick.Controls

Column {
    id: root
    property string keyText
    property bool value

    Text{
        id: mainText
        text:keyText
        wrapMode:Text.WordWrap
        maximumLineCount:5
        width:200
    }
    Text{
        text:value ? qsTr("Valid") : qsTr("Invalid")
        color: value ? "grey" : "red"
    }


}
