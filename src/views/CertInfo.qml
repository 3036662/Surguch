import QtQuick

Item {
    id: root
    anchors.fill: parent
    clip: true

    property var cert

    Rectangle {
        anchors.fill: parent
        color: "#fcfcfc"
    }

    Flickable {
        width: parent.width
        height: parent.height    
        topMargin: 10
        anchors.horizontalCenter: parent.horizontalCenter
        contentHeight: content.height * 1.1
        flickableDirection: Flickable.VerticalFlick

        Column {
            id: content

            width: parent.width - 20
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 5

            TextPairBool {
                width: parent.width
                keyText: qsTr("Status")
                value: root.cert !== undefined ? root.cert.trust_status : ""
            }            

            TextPair {
                width: parent.width
                keyText: qsTr("Date of issue")
                value: root.cert !== undefined ? root.cert.not_before_readable : ""
            }

            TextPair {
                width: parent.width
                keyText: qsTr("Expiration date")
                value: root.cert !== undefined ? root.cert.not_after_readable : ""
            }

            TextPair {
                width: parent.width
                keyText: qsTr("Serial number")
                value: root.cert !== undefined ? root.cert.serial : ""
            }

            TextPair {
                width: parent.width
                keyText: qsTr("Issuer")
                value: root.cert !== undefined ? root.cert.issuer : ""
            }

            TextPair {
                width: parent.width
                keyText: qsTr("Subject")
                value: root.cert !== undefined ? root.cert.subject : ""
            }

            TextPair {
                width: parent.width
                keyText: qsTr("Version")
                value: root.cert !== undefined ? root.cert.version : ""
            }

            TextPair {
                width: parent.width
                keyText: qsTr("Usage")
                value: {
                    if (root.cert === undefined) {
                        return ""
                    }
                    const key_usage = cert["key_usage"]
                    let val = ""
                    if (key_usage.length > 0 && key_usage.charAt(0) === "1") {
                        val += qsTr("Digital signature; ")
                    }
                    if (key_usage.length > 1 && key_usage.charAt(1) === "1") {
                        val += qsTr("nonRepudiation; ")
                    }
                    if (key_usage.length > 2 && key_usage.charAt(2) === "1") {
                        val += qsTr("keyEncipherment; ")
                    }
                    if (key_usage.length > 3 && key_usage.charAt(3) === "1") {
                        val += qsTr("dataEncipherment; ")
                    }
                    if (key_usage.length > 4 && key_usage.charAt(4) === "1") {
                        val += qsTr("keyAgreement; ")
                    }
                    if (key_usage.length > 5 && key_usage.charAt(5) === "1") {
                        val += qsTr("keyCertSign; ")
                    }
                    if (key_usage.length > 6 && key_usage.charAt(6) === "1") {
                        val += qsTr("cRLSign; ")
                    }
                    if (key_usage.length > 7 && key_usage.charAt(7) === "1") {
                        val += qsTr("encipherOnly; ")
                    }
                    if (key_usage.length > 8 && key_usage.charAt(8) === "1") {
                        val += qsTr("decipherOnly; ")
                    }
                    return val
                }
            }
        }
    }
}
