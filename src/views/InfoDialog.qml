import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import StyleSheet

Dialog {
    id: root

    property string version: "0.4.6"

    width: 510
    height: 130
    topPadding: StyleSheet.defaultPaddingV
    bottomPadding: StyleSheet.defaultPaddingV
    leftPadding: StyleSheet.defaultPaddingH
    rightPadding: StyleSheet.defaultPaddingH
    topMargin: StyleSheet.defaultMarginV
    bottomMargin: StyleSheet.defaultMarginV
    leftMargin: StyleSheet.defaultMarginH
    rightMargin: StyleSheet.defaultMarginH

    modal: true
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2

    RowLayout {
        anchors.fill:parent
        Image {
            source: "qrc:/basealt.svg"
            width: 100
            height: 100
            // Use sourceSize to control the rasterization size and avoid pixelation on initial scale
            sourceSize: Qt.size(width, height)
        }

        ColumnLayout {
            //anchors.fill: parent
            Layout.fillWidth: true

            Text {
                text: qsTr("Surguch - application for work with digital signature")
                font.family: "Noto Sans"
                font.pointSize: 10
                color: StyleSheet.font_color_extra
            }

            Text {
                text: qsTr("Version ") + root.version
                font.family: "Noto Sans"
                font.pointSize: 10
                color: StyleSheet.font_color_extra
            }

            Text {
                text: qsTr("Help ") + ("<a href='https://www.altlinux.org/%D0%A1%D1%83%D1%80%D0%B3%D1%83%D1%87'>%1</a>".arg(
                                           "https://www.altlinux.org/Сургуч"))
                textFormat: Text.RichText
                font.family: "Noto Sans"
                font.pointSize: 10
                color: StyleSheet.font_color_extra

                linkColor: "#1a73e8"
                font.underline: false

                MouseArea {
                    anchors.fill: parent
                    cursorShape: parent.hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor
                    hoverEnabled: true
                    onClicked: {
                        if (parent.hoveredLink) {
                            Qt.openUrlExternally(parent.hoveredLink)
                        }
                    }
                }
            }
        }
    }
}
