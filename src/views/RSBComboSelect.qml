import QtQuick
import QtQuick.Controls

ComboBox {
    id: control

    property bool item_selected: false

    width: parent.width
    textRole: ""
    valueRole: ""
    wheelEnabled: false

    onActivated: {
        displayText = model[currentIndex].title
        control.item_selected = true
    }

    delegate: ItemDelegate {
        id: delegate

        required property var model
        required property int index

        width: control.width
        contentItem: Text {
            text: delegate.model[control.textRole]
            font: control.font
            elide: Text.ElideRight
            verticalAlignment: Text.AlignVCenter
        }
        highlighted: control.highlightedIndex === index
    }

    contentItem: Text {
        leftPadding: 0
        rightPadding: control.indicator.width + control.spacing
        text: control.displayText
        font: control.font
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight

        // suppress a warning "positionToRectanglem is not a function"
        function positionToRectangle(pos) {
            return Qt.rect(0, 0, 0, 0)
        }
    }

    background: Rectangle {
        implicitWidth: 120
        implicitHeight: 40
        border.color: control.item_selected ? "lightGrey" : "red"
        border.width: control.visualFocus ? 2 : 1
        radius: 2
    }
}
