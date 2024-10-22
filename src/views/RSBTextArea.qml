import QtQuick
import QtQuick.Controls

TextArea {
    id: root
    background: Rectangle {
        border.color: root.text === "" ? "red" : "lightGrey"
    }
    width: parent.width
    selectByMouse: true
    wrapMode: Text.WordWrap
    placeholderText: qsTr("Placeholder text")
    text: profile_json !== undefined ? profile_json.title : ""
    placeholderTextColor: "grey"
}
