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

            BusyIndicator {
                id: busy_indicator
                leftPadding: 10
                running: true
                visible: !model.empty && !model.checkStatus
            }

            Column {
                width: parent.width
                anchors.verticalCenter: parent.verticalCenter

                Text {
                    width: parent.width - medal_icon.width - busy_indicator.width - 50
                    id: sigTitle
                    text: model.sigInfo
                    anchors.horizontalCenter: parent.horizontalCenter
                    elide: Text.ElideRight
                    wrapMode: Text.WordWrap
                    maximumLineCount: 3
                    font.pointSize: text.length > 40 ? 5 : 10
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
                id: medal_icon
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
            MouseArea {
                anchors.fill: parent

                onClicked: {
                    showSigData(JSON.stringify(model.sigData))
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
}
