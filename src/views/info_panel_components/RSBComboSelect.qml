import QtQuick
import QtQuick.Controls
import StyleSheet

ComboBox {
    id: control

    property bool item_selected: false

    width: parent.width
    textRole: ""
    valueRole: ""
    wheelEnabled: false
    font.family: "Noto Sans"

    popup.y: control.height

    onActivated: {
        displayText = model[currentIndex].title
        control.item_selected = true
    }

    delegate: ItemDelegate {
        id: delegate
        height: 25

        required property var model
        required property int index

        width: control.width
        contentItem: Item {

            width: delegate.width
            height: delegate.height

            Text {
                height: parent.height
                width: parent.width
                //text: delegate.model[control.textRole]
                text: control.model[index][control.textRole]
                font: control.font
                elide: Text.ElideRight
                verticalAlignment: Text.AlignVCenter
                color: StyleSheet.font_color_extra
            }
        }
        highlighted: control.highlightedIndex === index

        ToolTip {
            id: tooltip_obj
            background: Rectangle {
                color: StyleSheet.tooltip_background
            }
            contentItem: Text {
                text: tooltip_obj.text
                font.family: "Noto Sans"
                color: StyleSheet.font_color_extra
            }
            visible: control.highlightedIndex === index && text !== ""
            text: control.model[index]["tooltip"]
                  !== undefined ? control.model[index]["tooltip"] : ""
        }
    }

    contentItem: Text {
        leftPadding: 5
        rightPadding: control.indicator.width + control.spacing
        text: control.displayText
        font: control.font
        color: StyleSheet.font_color_extra
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight

        // suppress a warning "positionToRectangle is not a function"
        function positionToRectangle(pos) {
            return Qt.rect(0, 0, 0, 0)
        }
    }

    background: Rectangle {
        implicitWidth: 120
        implicitHeight: 29
        border.color: StyleSheet.slider_border_color
        border.width: control.visualFocus ? 2 : 1
        radius: 2
        color: StyleSheet.combo_box_background
    }
}
