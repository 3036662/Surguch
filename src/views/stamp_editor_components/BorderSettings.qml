import QtQuick
import QtQuick.Layouts
import StyleSheet
import QtQuick.Controls

ColumnLayout {
    id: root

    signal valueChanged();

    property alias border_width: borderWidth.value
    property alias radius: borderRadius.value

    RowLayout {
        Layout.fillWidth: true

        Text {
            text: qsTr("Stamp border width: ")
            font.family: StyleSheet.defaultFontFamily
            font.pointSize: StyleSheet.defaultTextPointSize
            color: StyleSheet.font_color_extra
        }

        Rectangle {
            Layout.fillWidth: true
        }

        Text {
            id: borderWidthText
            text: borderWidth.value
            font.family: StyleSheet.defaultFontFamily
            font.pointSize: StyleSheet.defaultTextPointSize
            color: StyleSheet.font_color_extra
        }
    }

    SettingSlider {
        id: borderWidth
        Layout.fillWidth: true
        snapMode: Slider.SnapOnRelease
        from: 0
        to: 20
        stepSize: 1

        onValueChanged: {
            root.valueChanged();
        }
    }

    // border radius
    RowLayout {
        Layout.fillWidth: true

        Text {
            text: qsTr("Stamp border radius: ")
            font.family: StyleSheet.defaultFontFamily
            font.pointSize: StyleSheet.defaultTextPointSize
            color: StyleSheet.font_color_extra
        }

        Rectangle {
            Layout.fillWidth: true
        }

        Text {
            id: borderRadiusText
            text: borderRadius.value
            font.family: StyleSheet.defaultFontFamily
            font.pointSize: StyleSheet.defaultTextPointSize
            color: StyleSheet.font_color_extra
        }
    }

    SettingSlider {
        id: borderRadius
        Layout.fillWidth: true
        snapMode: Slider.SnapOnRelease
        from: 1
        to: 70
        stepSize: 1

        onValueChanged: {
            root.valueChanged();
        }
    }
}
