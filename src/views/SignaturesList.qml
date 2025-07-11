import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import StyleSheet

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

            RowLayout {
                anchors.fill: parent
                spacing: 10

                Column {
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignVCenter

                    Text {
                        width: parent.width
                        id: sigTitle
                        text: model.sigInfo
                        anchors.horizontalCenter: parent.horizontalCenter
                        elide: Text.ElideRight
                        wrapMode: Text.WordWrap
                        maximumLineCount: 3
                        font.pointSize: text.length > 40 ? 5 : 10
                        font.family: "Noto Sans"
                        color: StyleSheet.font_color_extra
                    }
                    Text {
                        visible: model.empty
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: qsTr("Unsigned (empty)")
                        topPadding: 5
                        font.pointSize: sigTitle.font.pointSize / 1.5
                        font.family: "Noto Sans"
                        color: StyleSheet.font_color_extra
                    }
                }

                RowLayout {
                    Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
                    spacing: 5

                    BusyIndicator {
                        id: busy_indicator
                        leftPadding: 10
                        running: !model.empty && !model.checkStatus
                        visible: !model.empty && !model.checkStatus
                    }

                    Item {
                        id: medal_icon
                        width: 20
                        height: 20
                        Image {
                            width: 20
                            height: 20
                            source: model.checkStatus === false ? StyleSheet.medal_icon : (model.valid === true ? StyleSheet.medal_green_icon : StyleSheet.medal_pink_icon)
                        }
                    }
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
