import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ApplicationWindow {
    id: root
    width: 1000
    height: 480
    visible: true
    title: qsTr("Hello World")

    header: ToolBar {
        id: toolbar
        ColumnLayout {
            anchors.fill: parent
            Header {}
            HeaderSubBar {}
        }
    }

    background: Rectangle {
        color: "darkGrey"
    }
}
