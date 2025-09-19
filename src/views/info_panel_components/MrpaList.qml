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
                        leftPadding: 15
                        text: root.orgName(modelData)
                        anchors.horizontalCenter: parent.horizontalCenter
                        elide: Text.ElideRight
                        wrapMode: Text.WordWrap
                        maximumLineCount: 3
                        font.pointSize: text.length > 40 ? 5 : 10
                        font.family: "Noto Sans"
                        color: StyleSheet.font_color_extra
                    }
                }
            }
            MouseArea {
                anchors.fill: parent

                onClicked: {
                    console.warn("click show mrpa data")
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
