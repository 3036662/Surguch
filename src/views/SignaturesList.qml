import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ListView {
    id: root
    anchors.leftMargin: 5
    anchors.rightMargin: 5
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.fill: parent
    spacing: 3
    flickableDirection: Flickable.VerticalFlick

    model: siglistModel

    delegate: ColumnLayout {
        width: root.width
        height: 50

        Item {
            Layout.alignment: Qt.AlignVCenter
            Layout.fillWidth: true
            Layout.preferredHeight: 30

            BusyIndicator{
                leftPadding: 10
                running: true
                visible: !model.empty
            }

            Column {
                width: parent.width
                anchors.verticalCenter: parent.verticalCenter

                Text {
                    id: sigTitle
                    text: model.sigInfo
                    anchors.horizontalCenter: parent.horizontalCenter
                }
                Text {
                    visible: model.empty
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: qsTr("Unsigned (empty)")
                    topPadding: 5
                    font.pointSize: sigTitle.font.pointSize / 1.5
                }
            }
            Item {
                width: 20
                height: 20
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                Image {
                    width: 20
                    height: 20
                    source: model.checkStatus === false ? "qrc:/icons/medal-ribbon.svg" : (model.valid === true ? "qrc:/icons/medal-ribbon-green.svg" : "qrc:/icons/medal-ribbon-pink.svg")
                }
            }
        }
        Rectangle {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignBottom
            height: 1
            color: "#c3c3c3"
        }
    }

    Component.onCompleted: {
        pdfModel.signaturesFound.connect(siglistModel.updateSigList)
    }
}
