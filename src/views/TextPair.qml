import QtQuick
import QtQuick.Controls
Column {
    id:root

    property string keyText
    property string value
    property int textWidth

    width: parent.width

    Text{
        text:keyText
    }
    TextArea{
        background: Rectangle {
            border.color:"transparent"
        }

        width: parent.width
        readOnly: true
        selectByMouse: true
        wrapMode:Text.WordWrap
        //maximumLineCount: 5
        text:value
        color: "grey"
    }

}
