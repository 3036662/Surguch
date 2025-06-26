import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import StyleSheet

ColumnLayout {

    property int redoCount: 0
    property int undoCount: 0

    property alias searchDialog: searchDialog

        signal
    zoomInClicked
        signal
    zoomOutClicked

    signal zoomSelected(int newZoom)

    signal scrollToPage(int pageNumber)

        signal
    rotateClockwise
        signal
    rotateCounterClockWise
        signal
    showPreviews
        signal
    showCerts
        signal
    undoAction
        signal
    redoAction

    function changePageCount(newCount) {
        page_number.pageCount = newCount
        pageNumberInputValidator.top = newCount + 1
    }

    function changedCurrPage(newIndex) {
        page_number.currPage = newIndex
        pageNumberInput.text = newIndex
    }

    function updateZoomValue(zoom) {
        if (zoom === -1) {
            comboBoxZoom.currentIndex = -1
            comboBoxZoom.displayText = comboBoxZoom.model[0]
            return
        }
        comboBoxZoom.displayText = Math.round(zoom * 100) + "%"
        comboBoxZoom.currentIndex = -1
    }

    function disableZoom() {
        zoomButton.enabled = false
    }

    function enableZoom() {
        zoomButton.enabled = true
    }

    function enableZoomOut() {
        zoomOutButton.enabled = true
    }

    function disableZoomOut() {
        zoomOutButton.enabled = false
    }

    function setTagData(value) {
        rubberStampPutButton.tag_data = value
    }

    function enableTagButton() {
        console.debug("enabling tag button")
        rubberStampPutButton.down = false
        rubberStampPutButton.enabled = true
    }

    function clickTagButton() {
        rubberStampPutButton.enabled = true
        pdfListView.tagMode = !pdfListView.tagMode
        pdfListView.tagData = rubberStampPutButton.tag_data
        pdfModel.prepareImage(JSON.parse(rubberStampPutButton.tag_data))
        if (!rubberStampPutButton.down) {
            pdfListView.reserRotation()
        }
        rubberStampPutButton.down = !rubberStampPutButton.down
    }

    function updateHistory(undo, redo) {
        if (undo) {
            undoCount = undo
        }
        if (redo) {
            redoCount = redo
        }
    }

    function disableTagMode() {
        pdfListView.tagMode = false
        rubberStampPutButton.down = false
    }

    spacing: 1

    Rectangle {
        height: 1
        color: "grey"
        Layout.fillWidth: true
    }

    RowLayout {
        id: toolbar_subpanel
        spacing: 5
        ToolButton {
            flat: true
            display: AbstractButton.IconOnly
            icon.source: StyleSheet.book_icon
            icon.width: 20
            icon.height: 20
            leftPadding: 40
            rightPadding: 40

            onClicked: {
                showPreviews()
            }
        }

        HeaderToolSeparator {
        }

        ToolButton {
            flat: true
            display: AbstractButton.IconOnly
            icon.source: StyleSheet.pen_tool_icon
            icon.width: 20
            icon.height: 20
            leftPadding: 40
            rightPadding: 40

            onClicked: {
                showCerts()
            }
        }

        HeaderToolSeparator {
        }

        ToolButton {
            flat: true
            display: AbstractButton.IconOnly
            icon.source: StyleSheet.printer_icon
            icon.width: 20
            icon.height: 20
            leftPadding: 5
            rightPadding: 5

            onClicked: {
                printer.print(pdfListView.source, pdfListView.count,
                    pdfListView.landscape)
            }
        }

        HeaderToolSeparator {
        }

        ToolButton {
            flat: true
            display: AbstractButton.IconOnly
            icon.source: StyleSheet.arrow_down_icon
            icon.width: 20
            icon.height: 20
            leftPadding: 5
            rightPadding: 5

            onClicked: {
                if (page_number.currPage < pageNumberInputValidator.top) {
                    scrollToPage(page_number.currPage + 1)
                }
            }
        }

        ToolButton {

            flat: true
            display: AbstractButton.IconOnly
            icon.source: StyleSheet.arrow_up_icon
            icon.width: 20
            icon.height: 20
            leftPadding: 5
            rightPadding: 5

            onClicked: {
                if (page_number.currPage > pageNumberInputValidator.bottom) {
                    scrollToPage(page_number.currPage - 1)
                }
            }
        }

        TextField {
            id: pageNumberInput
            text: "1"
            maximumLength: 100
            Layout.preferredWidth: 50
            horizontalAlignment: TextInput.AlignHCenter
            font.family: "Noto Sans"

            validator: IntValidator {
                id: pageNumberInputValidator
                bottom: 1
                top: 10000
            }

            onAccepted: {
                let newIndex = Number(text)
                if (newIndex > 0) {
                    scrollToPage(newIndex)
                }
            }
        }

        Text {
            id: page_number
            property int pageCount: 1
            property int currPage: 1
            text: currPage + qsTr(" of ") + pageCount
            anchors.margins: 10
            font.family: "Noto Sans"
            color: StyleSheet.font_color_extra
        }

        HeaderToolSeparator {
        }

        ToolButton {
            flat: true
            display: AbstractButton.IconOnly
            icon.source: StyleSheet.arrow_back_icon
            icon.width: 20
            icon.height: 20
            leftPadding: 5
            rightPadding: 5

            onClicked: {
                rotateCounterClockWise()
            }
        }

        ToolButton {
            flat: true
            display: AbstractButton.IconOnly
            icon.source: StyleSheet.arrow_forward_icon
            icon.width: 20
            icon.height: 20
            leftPadding: 5
            rightPadding: 5

            onClicked: {
                rotateClockwise()
            }
        }

        HeaderToolSeparator {
        }

        ToolButton {
            id: zoomOutButton
            onClicked: {
                zoomOutClicked()
            }
            flat: true
            display: AbstractButton.IconOnly
            icon.source: StyleSheet.minus_circle_icon
            icon.width: 20
            icon.height: 20
            leftPadding: 5
            rightPadding: 5
        }

        ToolButton {
            id: zoomButton
            onClicked: {
                zoomInClicked()
            }
            flat: true
            display: AbstractButton.IconOnly
            icon.source: StyleSheet.plus_circle_icon
            icon.width: 20
            icon.height: 20
            leftPadding: 5
            rightPadding: 5
        }
        Row {
            Rectangle {
                width: 10
                height: parent.height
                color: "transparent"
            }
            ComboBox {
                id: comboBoxZoom

                onCurrentIndexChanged: {
                    let newZoom = 0
                    switch (currentIndex) {
                        case 0:
                            newZoom = -1 //auto
                            break
                        case 1:
                            newZoom = 75
                            break
                        case 2:
                            newZoom = 100
                            break
                        case 3:
                            newZoom = 125
                            break
                        case 4:
                            newZoom = 150
                            break
                    }
                    if (newZoom != 0) {
                        zoomSelected(newZoom)
                    }
                }

                Layout.alignment: Qt.AlignVCenter
                model: [qsTr("Automatic"), "75%", "100%", "125%", "150%"]
                currentIndex: 2
                implicitContentWidthPolicy: ComboBox.ContentItemImplicitWidth
                anchors.verticalCenter: parent.verticalCenter
                popup.y: comboBoxZoom.height
            }
            Rectangle {
                width: 10
                height: parent.height
                color: "transparent"
            }
        }

        // rubberStamps
        HeaderToolSeparator {
        }


        ToolButton {
            id: rubberStampPutButton

            property var tag_data

            enabled: !!tag_data
            flat: true
            icon.width: 20
            icon.height: 20
            leftPadding: 5
            rightPadding: 5
            icon.source: StyleSheet.tag_icon
            onClicked: {
                //console.debug("create tag")
                header.quitSignMode()
                pdfListView.tagMode = !pdfListView.tagMode
                pdfListView.tagData = tag_data
                pdfModel.prepareImage(JSON.parse(tag_data))
                if (!down) {
                    pdfListView.reserRotation()
                }
                down = !down
            }
        }

        Keys.onPressed: event => {
            if (event.key === Qt.Key_Escape
                && pdfListView.tagMode) {
                header.enableSignMode()
                pdfListView.tagMode = false
                rubberStampPutButton.down = false
                event.accepted = true
                return
            }
            event.accepted = false
        }

        ToolButton {
            id: rubberStampDialogButton

            flat: true
            icon.width: 20
            icon.height: 10
            leftPadding: 5
            rightPadding: 5
            topPadding: 5
            bottomPadding: 5
            icon.source: StyleSheet.chevron_down
            onClicked: {
                header.quitSignMode()
                if (rubberStampDialog.visible) {
                    rubberStampDialog.close()
                } else {
                    rubberStampDialog.open()
                }
            }
        }


        // search
        HeaderToolSeparator {
        }
        ToolButton {
            id: searchButton
            enabled: !pdfListView.signMode && !pdfListView.tagMode
            flat: true
            icon.source: StyleSheet.search_icon
            icon.width: 20
            icon.height: 20
            leftPadding: 5
            rightPadding: 5
            onClicked: searchDialog.open()
        }

        Rectangle {
            color: "transparent"
            Layout.fillHeight: true
            Layout.fillWidth: true
        }

        // ToolButton {
        //     flat: true
        //     display: AbstractButton.IconOnly
        //     icon.source: "qrc:/icons/fullscreen-custom.svg"
        //     icon.width: 20
        //     icon.height: 20
        //     leftPadding: 5
        //     rightPadding: 5
        //     Layout.alignment: Qt.AlignRight
        // }
    }

    SearchDialog {
        id: searchDialog
    }

    RubberStampDialog {
        id: rubberStampDialog
    }

    Shortcut {
        sequence: "Ctrl+F"
        onActivated: {
            searchDialog.open()
            searchDialog.focus = true;
        }
    }

    Shortcut {
        id: undoShortcut

        enabled: undoCount > 0
        sequence: "Ctrl+Z"
        onActivated: {
            console.warn("undo")
            undoAction()
            updateHistory()
        }
    }

    Shortcut {
        id: redoShortcut

        enabled: redoCount > 0
        sequence: "Ctrl+Y"
        onActivated: {
            console.warn("redo")
            redoAction()
        }
    }
}
