import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import StyleSheet

Dialog {
    id: searchDialog

    property bool needNewSearch: false
    property bool searchInProgress: false
    property int needlesCount: 0
    property int currentIndex: 0
    property string prev_needle

    signal searchRequired(string needle)

    signal jumpToNeedle(int needle_index)

    function searchCompleted(first_needle_page, total_needles) {
        console.warn("QML search completed")
        searchInProgress = false
        needlesCount = total_needles
        currentIndex = needlesCount > 0 ? 1 : 0
        if (needNewSearch) {
            console.warn("QML need new search")
            searchInProgress = true
            needNewSearch = false
            searchRequired(searchInput.text)
        }
    }

    width: 350
    height: 50
    x: searchButton.x - width
    y: parent.y
    modal: false
    closePolicy: Popup.CloseOnEscape

    background: Rectangle {
        color: StyleSheet.text_area_background
        radius: 5
        border.color: StyleSheet.slider_border_color
        border.width: 1
    }

    onOpened: {
        root_window.focusOwnerId = "searchDialog"
        searchInput.forceActiveFocus()
    }

    onClosed: {
        root_window.focusOwnerId = ""
        searchInput.text = ""
        currentIndex = 0
        if (!searchDialog.searchInProgress) {
            searchDialog.searchInProgress = true
            searchDialog.searchRequired(searchInput.text)
        } else {
            searchDialog.needNewSearch = true
        }
    }

    Row {
        anchors.verticalCenter: parent.verticalCenter
        height: 40

        Rectangle {
            id: root
            width: 160
            anchors.verticalCenter: parent.verticalCenter
            height: parent.height - 5
            color: "transparent"
            border.color: "transparent"
            border.width: 1
            radius: 4
            TextInput {
                id: searchInput

                clip: true
                anchors.fill: parent
                anchors.margins: 4
                maximumLength: 100
                color: StyleSheet.font_color_extra
                horizontalAlignment: TextInput.AlignHLeft
                verticalAlignment: TextInput.AlignVCenter
                focus: true

                onTextEdited: {
                    if (text === prev_needle) {
                        return
                    }
                    prev_needle = text
                    console.warn("QML edited")
                    console.warn("searchInProgress:" + searchDialog.searchInProgress)
                    if (!searchDialog.searchInProgress) {
                        searchDialog.searchInProgress = true
                        searchDialog.searchRequired(text)
                    } else {
                        searchDialog.needNewSearch = true
                    }
                    root_window.focusOwnerId = "searchDialog"
                    forceActiveFocus()
                }

                Keys.onReturnPressed: {
                    if (searchDialog.currentIndex < searchDialog.needlesCount) {
                        searchDialog.currentIndex += 1
                        jumpToNeedle(searchDialog.currentIndex - 1)
                    }
                }
            }
        }

        Rectangle {
            width: 60
            height: parent.height
            color: "transparent"

            Control {
                id: symCount
                height: parent.height
                width: childrenRect.width
                anchors.centerIn: parent
                Text {
                    text: currentIndex + "/" + needlesCount
                    color: StyleSheet.font_color_extra
                    anchors.verticalCenter: parent.verticalCenter
                    verticalAlignment: Text.AlignVCenter
                }
            }
        }

        ToolSeparator {
            padding: vertical ? 10 : 2
            topPadding: vertical ? 2 : 10
            bottomPadding: vertical ? 2 : 10

            contentItem: Rectangle {
                implicitWidth: parent.vertical ? 1 : 32
                implicitHeight: parent.vertical ? 32 : 1
                color: StyleSheet.icon_color
            }
        }

        Rectangle {
            width: 10
        }

        ToolButton {
            height: parent.height
            flat: true
            icon.source: StyleSheet.chevron_down
            icon.width: 20
            icon.height: 10
            leftPadding: 5
            topPadding: 5
            rightPadding: 5
            bottomPadding: 5

            onClicked: {
                if (searchDialog.currentIndex < searchDialog.needlesCount) {
                    searchDialog.currentIndex += 1
                    jumpToNeedle(searchDialog.currentIndex - 1)
                }
            }
        }

        ToolButton {
            height: parent.height
            flat: true
            icon.source: StyleSheet.chevron_up
            icon.width: 20
            icon.height: 10
            leftPadding: 5
            topPadding: 5
            rightPadding: 5
            bottomPadding: 5
            onClicked: {
                if (searchDialog.currentIndex > 1) {
                    searchDialog.currentIndex -= 1
                    jumpToNeedle(searchDialog.currentIndex - 1)
                }
            }
        }

        ToolButton {
            height: parent.height
            flat: true
            icon.source: StyleSheet.close_icon
            icon.width: 20
            icon.height: 20
            leftPadding: 5
            topPadding: 5
            rightPadding: 5
            bottomPadding: 5
            onClicked: {
                searchDialog.close()
            }
        }
    }
}
