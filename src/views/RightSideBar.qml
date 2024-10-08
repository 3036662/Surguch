import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root

    enum ShowState {
        Invisible,
        SigInfo,
        ProfileInfo
    }

    property int showState: RightSideBar.ShowState.SigInfo

    visible: root.showState = !RightSideBar.ShowState.Invisible
    Layout.maximumWidth: 400
    Layout.preferredWidth: 200
    Layout.minimumWidth: 200
    Layout.fillHeight: true
    Layout.fillWidth: true
    Layout.horizontalStretchFactor: 1

    Rectangle {
         anchors.fill: parent
         color: "white"
    }

    Flickable {
        width: parent.width
        height: parent.height
        contentHeight: 1500
        leftMargin:10
        rightMargin:10



        ToolButton {
            flat: true
            display: AbstractButton.IconOnly
            icon.source: "qrc:/icons/close_icon.svg"
            icon.width: 20
            icon.height: 20
            leftPadding: 5
            topPadding: 30
            rightPadding: 30
            anchors.top: parent.top
            anchors.right: parent.right

            onClicked: {
                root.showState = RightSideBar.ShowState.Invisible
                root.visible = false
            }
        }

        Column {
            width: parent.width

            // Text {
            //     text: qsTr("Information")
            //     font.weight: Font.DemiBold
            //     topPadding: 10
            // }
            Text {
                text: qsTr("Signature")
                font.weight: Font.DemiBold
                topPadding: 10
            }

            //////////////////////////////////////
            // signature
            RSideBarStatusMedal {
                title: qsTr("Status")
                value: false
            }

            Column {
                anchors.left: parent.left
                anchors.leftMargin: 20

                TextPairBool {
                    keyText: qsTr("Document integrity")
                    value: false
                }
                TextPairBool {
                    keyText: qsTr("Mathematical correctness")
                    value: false
                }
                TextPairBool {
                    keyText: qsTr("Certificate")
                    value: false
                }
                TextPairBool {
                    keyText: qsTr("Time stamp (TSP)")
                    value: false
                }
                TextPairBool {
                    keyText: qsTr("OCSP status")
                    value: false
                }
            }
            RightSBHorizontalDelimiter {
                width: parent.width
            }
            TextPair {
                keyText: qsTr("Date of creation")
                value: "00/00/0000 00:00:00 UTC"
            }
            RightSBHorizontalDelimiter {
                width: parent.width
            }
            TextPair {
                keyText: qsTr("CADES standart")
                value: "CADEX XXXXXXXX"
            }

            RightSideBarCertChain {
                width: parent.width
                title: qsTr("Certificate chain")
                items: [{
                        "key": "cert1",
                        "value": false
                    }, {
                        "key": "cert2",
                        "value": false
                    }, {
                        "key": "cert3",
                        "value": false
                    }]
            }

            //////////////////////////////////////
            // time stamp
            Text {
                text: qsTr("Time stamp (TSP)")
                font.weight: Font.DemiBold
                topPadding: 10
                bottomPadding: 10
            }

            RSideBarStatusMedal {
                title: qsTr("Status")
                value: false
            }
            RightSBHorizontalDelimiter {
                width: parent.width
            }
            TextPair {
                keyText: qsTr("Date of creation")
                value: "00/00/0000 00:00:00 UTC"
            }
            RightSBHorizontalDelimiter {
                width: parent.width
            }
            TextPair {
                keyText: qsTr("Serial number")
                value: "0000000000000000000000000000"
            }
            RightSBHorizontalDelimiter {
                width: parent.width
            }
            TextPair {
                keyText: qsTr("Policy id")
                value: "0.0.0.0.0"
            }
            RightSideBarCertChain {
                width: parent.width
                title: qsTr("Certificate chain")
                items: [{
                        "key": "cert1",
                        "value": false
                    }, {
                        "key": "cert2",
                        "value": false
                    }, {
                        "key": "cert3",
                        "value": false
                    }]
            }

            //////////////////////////////////////
            // OCSP Status
            Text {
                text: qsTr("OCSP status")
                font.weight: Font.DemiBold
                topPadding: 10
                bottomPadding: 10
            }
            RSideBarStatusMedal {
                title: qsTr("Status")
                value: false
            }
            RightSBHorizontalDelimiter {
                width: parent.width
            }
            TextPair {
                keyText: qsTr("Issue date")
                value: "00/00/0000 00:00:00 UTC"
            }
            RightSBHorizontalDelimiter {
                width: parent.width
            }
            TextPair {
                keyText: qsTr("Serial number")
                value: "000000000000000000"
            }
            RightSBHorizontalDelimiter {
                width: parent.width
            }
            TextPair {
                keyText: qsTr("Policy id")
                value: "0.0.0.0.0"
            }
            RightSideBarCertChain {
                width: parent.width
                title: qsTr("Certificate chain")
                items: [{
                        "key": "cert1",
                        "value": false
                    }, {
                        "key": "cert2",
                        "value": false
                    }, {
                        "key": "cert3",
                        "value": false
                    }]
            }

        }
    }
}
