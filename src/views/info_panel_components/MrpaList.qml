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


    /**
     * Safely retrieves a deeply nested property from an object.
     * @param {object} obj - The root object to search.
     * @param {...string} path - The sequence of keys representing the path.
     * @returns {*} - The value at the nested path, or undefined if any part is missing.
     */
    function getNestedValue(obj, ...path) {
        try {
            return path.reduce((acc, key) => acc[key], obj)
        } catch (_) {
            return undefined
        }
    }


    /**
     * Extracts the organization (grantor) name from MRPA data.
     * Tries multiple possible paths depending on the data structure.
     * Returns "Undefined grantor" (translated) if not found.
     * @param {object} mrpa_data - The MRPA document data.
     * @returns {string} - The organization name or a fallback text.
     */
    function orgName(mrpa_data) {
        return (getNestedValue(mrpa_data, "Документ", "Довер", "СвДоверит",
                               "Доверит", "РосОргДовер", "СвРосОрг", "@НаимОрг")
                || getNestedValue(mrpa_data, "Документ", "Довер",
                                  "СвДоверит", "Доверит",
                                  "РосОргДовер", "СВЮЛ", "СвЮЛЕИО", "@НаимОрг")
                || qsTr("Undefined grantor"))
    }

    delegate: ColumnLayout {
        width: root.width
        height: 70

        Item {
            Layout.alignment: Qt.AlignVCenter
            Layout.fillWidth: true
            Layout.preferredHeight: 70

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
