import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

ColumnLayout{
    id :root

    signal valueChanged();

    property real sliderWidth: 320

    property alias r: redColor.value
    property alias g: greenColor.value
    property alias b: blueColor.value

    width:parent.width

    ColorSlider{
        id: redColor

        labelText: "R";
        sliderWidth:root.sliderWidth
        bgColor:  "#ff0000"

        onValueChanged: {
             root.valueChanged();
        }
    }


    ColorSlider{
        id: greenColor

        labelText: "G";
        sliderWidth:root.sliderWidth
        bgColor:  "#00ff00"

        onValueChanged: {
             root.valueChanged();
        }
    }

    ColorSlider{
        id: blueColor

        labelText: "B";
        sliderWidth:root.sliderWidth
        bgColor:  "#0000ff"

        onValueChanged: {
             root.valueChanged();
        }
    }
}


