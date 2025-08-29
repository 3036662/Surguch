import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

ColumnLayout{
    id :root

    signal valueChanged();

    property alias r: redColor.value
    property alias g: greenColor.value
    property alias b: blueColor.value

    ColorSlider{
        id: redColor

        labelText: "R";
        bgColor:  "#ff0000"

        onValueChanged: {
             root.valueChanged();
        }
    }


    ColorSlider{
        id: greenColor

        labelText: "G";
        bgColor:  "#00ff00"

        onValueChanged: {
             root.valueChanged();
        }
    }

    ColorSlider{
        id: blueColor

        labelText: "B";
        bgColor:  "#0000ff"

        onValueChanged: {
             root.valueChanged();
        }
    }
}


