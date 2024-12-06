import QtQuick
import QtQuick.Controls

ComboBox {
    id: control

    property bool item_selected: false

    width: parent.width
    textRole: ""
    valueRole: ""
    wheelEnabled: false
    font.family: "Noto Sans"

    popup.y:control.height

    onActivated: {
        displayText = model[currentIndex].title
        control.item_selected = true
    }

    delegate: ItemDelegate {
        id: delegate
        height: 30

        required property var model
        required property int index


        width: control.width
        contentItem: Item{

            width:delegate.width
            height:delegate.height

            Text {
                height: parent.height
                width:  parent.width
                //text: delegate.model[control.textRole]
                text:  control.model[index][control.textRole]
                font: control.font
                elide: Text.ElideRight
                verticalAlignment: Text.AlignVCenter

            }
        }
        highlighted: control.highlightedIndex === index

        ToolTip {
            id: tooltip_obj
            visible: control.highlightedIndex === index && text!==""
            text: control.model[index]["tooltip"]!==undefined ?
                          control.model[index]["tooltip"] : "";
        }
    }

    contentItem: Text {
        leftPadding: 5
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
