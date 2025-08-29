import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import StyleSheet

RowLayout {
    id:root

    signal closeClicked;

    property string labelText


    Label {
        text: labelText
        font.weight: Font.DemiBold
        topPadding: 5
        bottomPadding: 5
        font.family: "Noto Sans"
    }

    Rectangle {
        Layout.fillWidth: true
    }

    ToolButton {
        flat: true
        display: AbstractButton.IconOnly
        icon.width: 20
        icon.height: 20
        leftPadding: 10
        rightPadding: 10
        topPadding: 10
        bottomPadding: 10
        font.family: "Noto Sans"
        icon.source: StyleSheet.close_icon

        onClicked: {
            root.closeClicked();
        }
    }
}
