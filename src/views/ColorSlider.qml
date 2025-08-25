import QtQuick
import QtQuick.Layouts
import StyleSheet
import QtQuick.Controls

RowLayout {
    id: root
    property string labelText
    property real sliderWidth: 320
    property alias bgColor: color.back_color
    property alias gradStart: color.gradient_start
    property alias value: color.value

     onWidthChanged: {
         console.warn("RGB sliders column width:"+width)
     }

    Layout.fillWidth: true

    SettingSlider {
        id: color

        //Layout.maximumWidth:  root.sliderWidth
        Layout.fillWidth: true
        snapMode: Slider.SnapOnRelease
        from: 0
        to: 255
        stepSize: 1
        gradient_start: "#000000"
    }


    Text {
        text: root.labelText + " " + color.value
        width: 50
        font.family: "Noto Sans"
        color: StyleSheet.font_color_extra
        Layout.leftMargin: 5
    }
}
