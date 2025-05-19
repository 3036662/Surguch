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

    width: 325
    height: 50
    x: searchButton.x - width / 2
    y: parent.y
    modal: false
    closePolicy: Popup.CloseOnEscape

    onOpened: {
        root_window.focusOwnerId = "searchDialog"
        searchInput.forceActiveFocus()
    }

    onClosed: {
        root_window.focusOwnerId = ""
        searchInput.text = ""
        currentIndex = 0
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
            border.color: "#caccce"
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
            width: 10
            height: parent.height
            color: "transparent"
        }
        Control {
            height: parent.height
            width: childrenRect.width
            Text {
                text: currentIndex + "/" + needlesCount
                color: StyleSheet.font_color_extra
                anchors.verticalCenter: parent.verticalCenter
                verticalAlignment: Text.AlignVCenter
            }
        }
        Rectangle {
            width: 10
            height: parent.height
            color: "transparent"
        }

        ToolButton {
            height: parent.height
            flat: true
            display: AbstractButton.IconOnly
            icon.source: StyleSheet.chevron_down
            icon.width: 20
            icon.height: 20
            leftPadding: 0
            topPadding: 0
            rightPadding: 0
            bottomPadding: 0
            width: 25

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
            display: AbstractButton.IconOnly
            icon.source: StyleSheet.chevron_up
            icon.width: 20
            icon.height: 20
            leftPadding: 0
            topPadding: 0
            rightPadding: 0
            bottomPadding: 0
            width: 25
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
            display: AbstractButton.IconOnly
            icon.source: StyleSheet.close_icon
            icon.width: 20
            icon.height: 20
            leftPadding: 0
            topPadding: 0
            rightPadding: 0
            bottomPadding: 0
            width: 25
            onClicked: {
                searchDialog.close()
            }
        }
    }
}
