import QtQuick

Column {

    property string keyText
    property string value

    Text {
        text: keyText
    }
    Text {
        text: {
            if (value === "no_field") {
                return qsTr("Field is absent")
            }
            if (value === "no_check") {
                return qsTr("Not checked")
            }
            if (value === "failed") {
                return qsTr("Failed to check")
            }
            if (value === "ok") {
                return qsTr("Valid")
            }
            if (value === "bad") {
                return qsTr("Invalid")
            }
        }
        color: {
            if (value === "no_field" || value === "no_check") {
                return "grey"
            }
            if (value === "failed") {
                return "brown"
            }
            if (value === "ok") {
                return "green"
            }
            if (value === "bad") {
                return "red"
            }
        }
    }
}
