import QtQuick
import QtQuick.Controls
import QtQuick.Layouts


ApplicationWindow {
    id:root
    width: 640
    height: 480
    visible: true
    title: qsTr("Hello World")

    header: ToolBar{
        id:toolbar

        RowLayout{
            id: toolbar_layout
            anchors.fill: parent
            spacing: 10

            ToolButton{
                text: qsTr("Open")
                flat:true
                display:AbstractButton.TextBesideIcon
                icon.source: "qrc:/icons/file_plus.svg"
                icon.width: 50
                icon.height: 50

            }

        }
    }



    background:  Rectangle{
        color:"darkGrey"
    }
}
