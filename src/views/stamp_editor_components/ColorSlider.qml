import QtQuick
import QtQuick.Layouts
import StyleSheet
import QtQuick.Controls

RowLayout {
    id: root
    property string labelText    
    property alias bgColor: color.back_color
    property alias gradStart: color.gradient_start
    property alias value: color.value

    Layout.fillWidth: true

    SettingSlider {
        id: color

        Layout.fillWidth: true
        snapMode: Slider.SnapOnRelease
        from: 0
        to: 255
        stepSize: 1
        gradient_start: "#000000"
    }


    Text {
        text: root.labelText + " " + color.value
        Layout.preferredWidth:  40
        font.family: StyleSheet.defaultFontFamily
        font.pointSize: StyleSheet.defaultTextPointSize
        color: StyleSheet.font_color_extra
        Layout.leftMargin: 5
    }
}
