import QtQuick
import QtQuick.Controls

Flickable {
    width: parent.width
    height: parent.height
    contentHeight: 1500
    leftMargin: 10
    rightMargin: 10

    RSBCloseButton {}

    Column {
        width: parent.width

        Text {
            text: qsTr("Signature")
            font.weight: Font.DemiBold
            topPadding: 10
        }

        //////////////////////////////////////
        // signature
        RSideBarStatusMedal {
            title: qsTr("Status")
            value: jsonData !== undefined && jsonData.signature.status
        }

        Column {
            anchors.left: parent.left
            anchors.leftMargin: 20

            TextPairStatus {
                keyText: qsTr("Document integrity")
                value: jsonData ? jsonData.signature.integrity : "no_check"
            }
            TextPairStatus {
                keyText: qsTr("Mathematical correctness")
                value: jsonData !== undefined ? jsonData.signature.math_correct : "no_check"
            }
            TextPairStatus {
                keyText: qsTr("Certificate")
                value: jsonData !== undefined ? jsonData.signature.certificate_ok : "no_check"
            }
            TextPairStatus {
                keyText: qsTr("Time stamp (TSP)")
                value: jsonData !== undefined ? jsonData.signature.timestamp_ok : "no_check"
            }
            TextPairStatus {
                keyText: qsTr("OCSP status")
                value: jsonData !== undefined ? jsonData.signature.ocsp_ok : "no_check"
            }
        }
        RightSBHorizontalDelimiter {
            width: parent.width
        }
        TextPair {
            keyText: qsTr("Date of creation")
            value: jsonData !== undefined ? jsonData.signature.signing_time : ""
        }
        RightSBHorizontalDelimiter {
            width: parent.width
        }
        TextPair {
            keyText: qsTr("CADES standart")
            value: jsonData !== undefined ? jsonData.signature.cades_type : ""
        }

        // in case number of chains > 1
        Repeater {
            model: jsonData !== undefined ? jsonData.signers_chain : []
            RightSideBarCertChain {
                width: parent.width
                title: qsTr("Certificate chain")
                items: modelData.certs
                status: modelData.trust_status
            }
        }

        //////////////////////////////////////
        // time stamp
        Repeater {

            model: root.jsonData !== undefined ? root.jsonData.tsp_info : []

            Column {
                width: root.width
                required property var modelData
                Text {
                    text: qsTr("Time stamp (TSP)")
                    font.weight: Font.DemiBold
                    topPadding: 10
                    bottomPadding: 10
                }
                RSideBarStatusMedal {
                    title: qsTr("Status")
                    value: modelData.result ? "ok" : "bad"
                }

                RightSBHorizontalDelimiter {
                    width: parent.width
                }
                TextPair {
                    keyText: qsTr("Date of creation")
                    value: modelData.tst_content.gen_time_readable
                }
                RightSBHorizontalDelimiter {
                    width: parent.width
                }
                TextPair {
                    keyText: qsTr("Serial number")
                    value: modelData.tst_content.serial
                }
                RightSBHorizontalDelimiter {
                    width: parent.width
                }
                TextPair {
                    keyText: qsTr("Policy id")
                    value: modelData.tst_content.policy
                }

                Repeater {
                    model: modelData.chains !== undefined ? modelData.chains : []
                    Repeater {
                        required property var modelData
                        model: modelData
                        RightSideBarCertChain {
                            required property var modelData
                            width: parent.width
                            title: qsTr("TSP Certificate chain")
                            items: modelData.certs
                            status: modelData.trust_status
                        }
                    }
                }
            }
        }

        //////////////////////////////////////
        // OCSP Status
        Repeater {
            model: root.jsonData !== undefined ? root.jsonData.ocsp_info : []
            Column {
                width: root.width
                Text {
                    text: qsTr("OCSP status")
                    font.weight: Font.DemiBold
                    topPadding: 10
                    bottomPadding: 10
                }
                Repeater {
                    model: modelData.tbs_response_data.responses
                           !== undefined ? modelData.tbs_response_data.responses : []
                    Column {
                        width: root.width
                        RSideBarStatusMedal {
                            title: qsTr("Certificate status")
                            value: modelData.cert_status === "good"
                        }
                        RightSBHorizontalDelimiter {
                            width: parent.width
                        }
                        TextPair {
                            keyText: qsTr("Issue date")
                            value: modelData.this_update_readable
                        }

                        RightSBHorizontalDelimiter {
                            width: parent.width
                        }
                        TextPair {
                            keyText: qsTr("Certificate serial number")
                            value: modelData.cert_serial
                        }
                    }
                }
                Repeater {
                    model: modelData.ocsp_chains
                    RightSideBarCertChain {
                        width: parent.width
                        title: qsTr("Certificate chain")
                        items: modelData.certs
                        status: modelData.trust_status
                    }
                }
            }
        }

        // ByteRange analasys
        TextPairBool {
            id: fullCoverageStatus
            keyText: qsTr("The signature covers the entire document")
            value: jsonData !== undefined ? jsonData.full_coverage : false;
        }
        TextPairBool {
            visible:!fullCoverageStatus.value
            keyText: qsTr("It is possible to open a signed version")
            value: jsonData !== undefined ? jsonData.can_be_casted_to_full_coverage  : false;
        }

    }
}
