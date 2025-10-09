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
    model: mrpaListData

    signal closeClicked

    header: Item {
        width: root.width
        height: 30

        RSBCloseButton {
            anchors.top: parent.top
            anchors.right: parent.right
            anchors.margins: 5
            onClicked: root.closeClicked()
        }
    }

    function g(o, ...p) {
        try {
            return p.reduce((a, k) => a[k], o)
        } catch (_) {
            return undefined
        }
    }
    function orgName(o) {
        return g(o, "Документ", "Довер", "СвДоверит", "Доверит", "РосОргДовер",
                 "СвРосОрг", "@НаимОрг") || g(o, "Документ", "Довер",
                                              "СвДоверит", "Доверит",
                                              "РосОргДовер", "СВЮЛ",
                                              "СвЮЛЕИО", "@НаимОрг")
                || qsTr("Undefined grantor")
    }
    function date(o) {
        return g(o, "Документ", "Довер", "СвДов", "@ДатаВыдДовер") || ""
    }
    function powersCount(o) {
        const m = g(o, "Документ", "Довер", "СвПолн", "МашПолн")
        return Array.isArray(m) ? m.length : (m ? 1 : 0)
    }

    delegate: ColumnLayout {
        width: root.width
        height: 70

        Item {
            Layout.alignment: Qt.AlignVCenter
            Layout.fillWidth: true
            Layout.preferredHeight: 50

            RowLayout {
                anchors.fill: parent
                spacing: 10

                Column {
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignVCenter

                    Text {
                        width: parent.width
                        id: sigTitle
                        leftPadding: 15
                        text: root.orgName(modelData)
                        anchors.horizontalCenter: parent.horizontalCenter
                        elide: Text.ElideRight
                        wrapMode: Text.WordWrap
                        maximumLineCount: 3
                        font.family: "Noto Sans"
                        font.pixelSize: 12
                        color: StyleSheet.font_color_extra
                    }
                }
            }
            MouseArea {
                anchors.fill: parent

                onClicked: {
                    showMrpaData(modelData)
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
