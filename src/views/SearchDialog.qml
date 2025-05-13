import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Dialog {
    id: searchDialog

    property bool needNewSearch: false
    property bool searchInProgress: false

    width: 300
    height: 50
    x: searchButton.x - width / 2
    y: parent.y
    modal: false
    closePolicy: Popup.CloseOnEscape

    Row {
        anchors.verticalCenter: parent.verticalCenter
        height: 40

        Rectangle {
            width: 150
            anchors.verticalCenter: parent.verticalCenter
            height: parent.height - 5
            color: "transparent"
            border.color: "#caccce"
            border.width: 1
            radius: 4
            TextInput {
                clip: true
                anchors.fill: parent
                anchors.margins: 4
                maximumLength: 100
                horizontalAlignment: TextInput.AlignHLeft
                verticalAlignment: TextInput.AlignVCenter

                onTextEdited:{
                    console.warn("EDITED");
                }
            }
        }

        Rectangle {
            width: 10
            height: parent.height
            color: "transparent"
        }
        Control {
            height: parent.height
            width: childrenRect.width
            Text {
                text: "0/0"
                anchors.verticalCenter: parent.verticalCenter
                verticalAlignment: Text.AlignVCenter
            }
        }
        Rectangle {
            width: 10
            height: parent.height
            color: "transparent"
        }

        ToolButton {
            height: parent.height
            flat: true
            display: AbstractButton.IconOnly
            icon.source: "qrc:/icons/search-next.svg"
            icon.width: 20
            icon.height: 20
            icon.color: "black"
            leftPadding: 0
            topPadding: 0
            rightPadding: 0
            bottomPadding: 0
            width: 25
        }

        ToolButton {
            height: parent.height
            flat: true
            display: AbstractButton.IconOnly
            icon.source: "qrc:/icons/search-previous.svg"
            icon.width: 20
            icon.height: 20
            icon.color: "black"
            leftPadding: 0
            topPadding: 0
            rightPadding: 0
            bottomPadding: 0
            width: 25
        }

        ToolButton {
            height: parent.height
            flat: true
            display: AbstractButton.IconOnly
            icon.source: "qrc:/icons/close_icon.svg"
            icon.width: 20
            icon.height: 20
            icon.color: "black"
            leftPadding: 0
            topPadding: 0
            rightPadding: 0
            bottomPadding: 0
            width: 25
            onClicked: {
                searchDialog.close()
            }
        }
    }
}
