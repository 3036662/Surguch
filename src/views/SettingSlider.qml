import QtQuick
import QtQuick.Controls
import StyleSheet

Slider {
    id: control

    property color back_color: StyleSheet.slider_fill_color
    property color gradient_start: StyleSheet.slider_fill_color

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitHandleWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitHandleHeight + topPadding + bottomPadding)

    padding: 6

    handle: Rectangle {
        x: control.leftPadding + (control.horizontal ? control.visualPosition * (control.availableWidth - width) : (control.availableWidth - width) / 2)
        y: control.topPadding
           + (control.horizontal ? (control.availableHeight - height) / 2 : control.visualPosition
                                   * (control.availableHeight - height))
        implicitWidth: 20
        implicitHeight: 20
        radius: width / 2
        color: StyleSheet.slider_handle_color
        border.width: control.visualFocus ? 4 : 2
        border.color: StyleSheet.slider_border_color
    }

    background: Rectangle {
        x: control.leftPadding + (control.horizontal ? 0 : (control.availableWidth - width) / 2)
        y: control.topPadding + (control.horizontal ? (control.availableHeight - height) / 2 : 0)
        implicitWidth: control.horizontal ? 200 : 5
        implicitHeight: control.horizontal ? 5 : 200
        width: control.horizontal ? control.availableWidth : implicitWidth
        height: control.horizontal ? implicitHeight : control.availableHeight
        radius: 3
        scale: control.horizontal && control.mirrored ? -1 : 1

        gradient: Gradient {
            orientation: control.horizontal ? Gradient.Horizontal : Gradient.Vertical
            GradientStop {
                position: 0.0
                color: control.gradient_start
            }
            GradientStop {
                position: 1.0
                color: control.back_color
            }
        }
    }
}
