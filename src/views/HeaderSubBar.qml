import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ColumnLayout {

    signal zoomInClicked
    signal zoomOutClicked
    signal zoomSelected(int newZoom)
    signal scrollToPage(int pageNumber)
    signal rotateClockwise()
    signal rotateCounterClockWise()

    function changePageCount(newCount) {
        page_number.pageCount = newCount
        pageNumberInputValidator.top = newCount+1
    }
    function changedCurrPage(newIndex) {
        page_number.currPage = newIndex
        pageNumberInput.text = newIndex
    }

    function updateZoomValue(zoom) {
        comboBoxZoom.displayText = Math.round(zoom * 100) + "%"
        comboBoxZoom.currentIndex = -1
    }

    function disableZoom(){
        zoomButton.enabled=false
    }

    function enableZoom(){
        zoomButton.enabled=true
    }

    function enableZoomOut(){
        zoomOutButton.enabled=true
    }

    function disableZoomOut(){
        zoomOutButton.enabled=false
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
            icon.source: "qrc:/icons/book.svg"
            icon.width: 20
            icon.height: 20
            leftPadding: 40
            rightPadding: 40
        }

        HeaderToolSeparator {}

        ToolButton {
            flat: true
            display: AbstractButton.IconOnly
            icon.source: "qrc:/icons/pen_tool.svg"
            icon.width: 20
            icon.height: 20
            leftPadding: 40
            rightPadding: 40
        }

        HeaderToolSeparator {}

        ToolButton {
            flat: true
            display: AbstractButton.IconOnly
            icon.source: "qrc:/icons/arrow-circle-down.svg"
            icon.width: 20
            icon.height: 20
            leftPadding: 5
            rightPadding: 5

            onClicked: {
                if (page_number.currPage<pageNumberInputValidator.top){
                    scrollToPage(page_number.currPage+1)
                }
            }
        }

        ToolButton {
            flat: true
            display: AbstractButton.IconOnly
            icon.source: "qrc:/icons/arrow-circle-up.svg"
            icon.width: 20
            icon.height: 20
            leftPadding: 5
            rightPadding: 5

            onClicked: {
                if (page_number.currPage>pageNumberInputValidator.bottom){
                    scrollToPage(page_number.currPage-1)
                }
            }
        }

        TextField {
            id: pageNumberInput
            text: "1"
            maximumLength: 100
            Layout.preferredWidth: 50
            horizontalAlignment: TextInput.AlignHCenter

            validator: IntValidator {
                id: pageNumberInputValidator
                bottom: 1
                top: 10000
            }

            onAccepted: {
                let newIndex=Number(text)
                if (newIndex>0){
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
        }

        HeaderToolSeparator {}

        ToolButton {
            flat: true
            display: AbstractButton.IconOnly
            icon.source: "qrc:/icons/arrow_back_curve.svg"
            icon.width: 20
            icon.height: 20
            leftPadding: 5
            rightPadding: 5

            onClicked: {
                rotateCounterClockWise();
            }
        }

        ToolButton {
            flat: true
            display: AbstractButton.IconOnly
            icon.source: "qrc:/icons/arrow_forward_curve.svg"
            icon.width: 20
            icon.height: 20
            leftPadding: 5
            rightPadding: 5

            onClicked: {
                rotateClockwise();
            }
        }

        HeaderToolSeparator {}

        ToolButton {
            id:zoomOutButton
            onClicked: {
                zoomOutClicked()
            }
            flat: true
            display: AbstractButton.IconOnly
            icon.source: "qrc:/icons/minus-circle.svg"
            icon.width: 20
            icon.height: 20
            leftPadding: 5
            rightPadding: 5
        }

        ToolButton {
            id:zoomButton
            onClicked: {
                zoomInClicked()
            }
            flat: true
            display: AbstractButton.IconOnly
            icon.source: "qrc:/icons/plus-circle.svg"
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
                        newZoom = 100
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
                currentIndex: 0
                implicitContentWidthPolicy: ComboBox.ContentItemImplicitWidth
                anchors.verticalCenter: parent.verticalCenter
            }
            Rectangle {
                width: 10
                height: parent.height
                color: "transparent"
            }
        }

        HeaderToolSeparator {}

        Row {
            Rectangle {
                width: 10
                height: parent.height
                color: "transparent"
            }
            TextField {
                placeholderText: qsTr("Search")
                maximumLength: 100
                Layout.preferredWidth: 150
                horizontalAlignment: TextInput.AlignLeft
                rightPadding: 30
                Button {
                    flat: true
                    icon.source: "qrc:/icons/search-custom.svg"
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    height: 24
                    smooth: true
                    anchors.right: parent.right
                }
            }
        }
        Rectangle {
            color: "transparent"
            Layout.fillHeight: true
            Layout.fillWidth: true
        }

        ToolButton {
            flat: true
            display: AbstractButton.IconOnly
            icon.source: "qrc:/icons/fullscreen-custom.svg"
            icon.width: 20
            icon.height: 20
            leftPadding: 5
            rightPadding: 5
            Layout.alignment: Qt.AlignRight
        }
    }
}
