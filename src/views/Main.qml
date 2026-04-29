import QtQuick
import QtQuick.Controls
//import QtQuick.Dialogs
import QtQuick.Layouts
import Qt.labs.platform as Labs
import alt.pdfcsp.pdfModel
import alt.pdfcsp.signatureCreator
import alt.pdfcsp.tagCreator
import alt.pdfcsp.profilesModel
import alt.pdfcsp.rubberStampModel
import alt.pdfcsp.signaturesListModel
import alt.pdfcsp.printerLauncher
import alt.pdfcsp.surguchLauncher
import alt.pdfcsp.fileTreeModel
import StyleSheet
import alt.pdfcsp.eventFilterInstaller
import alt.pdfcsp.wheelFilter
import alt.pdfcsp.surguchTranslator
import "EventsConnect.js" as EventsConnect

ApplicationWindow {
    id: root_window

    enum ShowType {
        Empty,
        Pdf,
        Files
    }

    property int showType: Main.ShowType.Empty

    width: 1000
    height: 480
    minimumWidth: 800
    minimumHeight: 600
    visible: true
    visibility: Window.Maximized
    title: qsTr("Surguch")

    property string focusOwnerId

    // --------------------------------------
    // header
    header: ToolBar {
        id: toolbar
        topPadding: 2
        ColumnLayout {
            anchors.fill: parent
            spacing: 2
            Header {
                id: header

                onChangeShowType: newType => {
                                      showType = newType
                                      if (showType === Main.ShowType.Pdf) {
                                          pdfDropArea.enabled = true
                                          fileDropArea.enabled = false
                                          pdfDropArea.width = parent.width
                                          fileTreeModel.deleteTree()                                            
                                          header.enableSignMode()
                                      }
                                      if (showType === Main.ShowType.Files) {
                                          pdfDropArea.enabled = false
                                          fileDropArea.enabled = true
                                          fileDropArea.width = parent.width
                                      }
                                  }
            }
            HeaderSubBar {
                id: headerSubBar
                visible: pdfListView.source !== ""
                         && showType === Main.ShowType.Pdf
            }

            FileModeHeaderSubBar {
                id: fileModeHeaderSubBar
                visible: showType === Main.ShowType.Files
            }
        }
    }

    // --------------------------------------
    // body
    DropArea {
        id: pdfDropArea
        width: parent.width / 2
        height: parent.height
        anchors {
            left: parent.left
            top: parent.top
            bottom: parent.bottom
        }

        onDropped: drop => {
                       fileDropArea.enabled = false
                       width = parent.width
                       let currentFile = Qt.resolvedUrl(drop.urls[0])
                       showType = Main.ShowType.Pdf
                       pdfListView.openFile(currentFile)
                       leftSideBar.source = currentFile
                       rightSideBar.showState = RightSideBar.ShowState.Invisible
                       if (drop.urls.length > 1) {
                           for (var i = 1; i < drop.urls.length; i++) {
                               let fileUrl = Qt.resolvedUrl(drop.urls[i])
                               launcher.launchSurguch(fileUrl)
                           }
                       }
                   }
    }

    DropArea {
        id: fileDropArea

        width: parent.width / 2
        height: parent.height
        anchors {
            right: parent.right
            top: parent.top
            bottom: parent.bottom
        }

        onDropped: drop => {
                       fileTreeModel.addNode(drop.urls)
                       pdfDropArea.enabled = false
                       width = parent.width
                       showType = Main.ShowType.Files
                   }
    }

    RowLayout {
        anchors.fill: parent
        spacing: 0

        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true
            visible: showType === Main.ShowType.Empty

            Row {
                anchors.centerIn: parent
                spacing: parent.width * 0.05

                Rectangle {
                    id: pdfZone

                    width: root_window.width / 6
                    height: width
                    radius: 6
                    color: "transparent"
                    border.width: pdfDropArea.containsDrag ? 2 : 1
                    border.color: pdfDropArea.containsDrag
                                  || pdfRectangle.containsMouse ? StyleSheet.slider_border_color : "#c7c7c7"

                    Column {
                        anchors.centerIn: parent
                        spacing: 8

                        Image {
                            source: StyleSheet.file_text_big_icon
                            width: pdfZone.width * 0.25
                            height: width
                            anchors.horizontalCenter: parent.horizontalCenter
                            fillMode: Image.PreserveAspectFit
                        }

                        Text {
                            text: qsTr("PDF\nDrag&Drop")
                            font.pixelSize: 14
                            font.family: "Noto Sans"
                            horizontalAlignment: Text.AlignHCenter
                            wrapMode: Text.WordWrap
                            color: StyleSheet.font_color_extra
                        }
                    }

                    MouseArea {
                        id: pdfRectangle
                        anchors.fill: parent
                        enabled: showType === Main.ShowType.Empty
                        hoverEnabled: true

                        onClicked: {
                            header.openPdfDialog()
                        }
                    }
                }

                Rectangle {
                    id: fileZone

                    width: root_window.width / 6
                    height: width
                    radius: 6
                    color: "transparent"
                    border.width: fileDropArea.containsDrag ? 2 : 1
                    border.color: fileDropArea.containsDrag
                                  || fileRectangle.containsMouse ? StyleSheet.slider_border_color : "#c7c7c7"

                    Column {
                        anchors.centerIn: parent
                        spacing: 8

                        Image {
                            source: StyleSheet.file_simple_big_icon
                            width: fileZone.width * 0.25
                            height: width
                            anchors.horizontalCenter: parent.horizontalCenter
                            fillMode: Image.PreserveAspectFit
                        }

                        Text {
                            text: qsTr("File\nDrag&Drop")
                            font.pixelSize: 14
                            font.family: "Noto Sans"
                            horizontalAlignment: Text.AlignHCenter
                            wrapMode: Text.WordWrap
                            color: StyleSheet.font_color_extra
                        }
                    }

                    MouseArea {
                        id: fileRectangle
                        anchors.fill: parent
                        enabled: showType === Main.ShowType.Empty
                        hoverEnabled: true

                        onClicked: {
                            header.openTreeDialog()
                        }
                    }
                }
            }
        }

        LeftSideBar {
            id: leftSideBar
            visible: showType === Main.ShowType.Pdf
            Layout.alignment: Qt.AlignRight
        }
        PdfListView {
            id: pdfListView
            Layout.preferredWidth: root_window.width - 500
            visible: showType === Main.ShowType.Pdf
        }

        FileTreeView {
            id: fileTreeView

            property int sizeColumn: fileModeHeaderSubBar.sizeColumn
            property int editColumn: fileModeHeaderSubBar.editColumn
            property int signColumn: fileModeHeaderSubBar.signColumn
            property int mrpaColumn: fileModeHeaderSubBar.mrpaColumn
            property int deleteColumn: fileModeHeaderSubBar.deleteColumn

            Layout.preferredWidth: root_window.width - 310
            Layout.maximumWidth: root_window.width - 310
            Layout.minimumWidth: root_window.width - 310
            visible: showType === Main.ShowType.Files
            Layout.rightMargin: 0
        }

        RightSideBar {
            id: rightSideBar
            Layout.alignment: Qt.AlignLeft
            Layout.leftMargin: 0
        }
    }

    // --------------------------------------
    // footer
    footer: Pane {
        id: footer_frame
        width: parent.width
        height: 10
        ScrollBar {
            id: horizontalScroll
            hoverEnabled: true
            active: hovered || pressed
            orientation: Qt.Horizontal
            size: contWidth > 0 ? pdfListView.width / contWidth : 0
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            visible: contWidth > pdfListView.width
            width: pdfListView.width
            property int contWidth: 0

            // move the listview when the scroll was moved
            onPositionChanged: {
                pdfListView.contentX = position * contWidth
                pdfListView.hScrollPos = position
                //console.warn(position+ " / " +  pdfListView.contentX)
            }

            // page width changed
            function setContentWidth(w) {
                size = pdfListView.width / (w > 0 ? w : 1)
                contWidth = w
            }

            // listview flicked
            function updateScrollPosition(newPosition) {
                if (visible) {
                    position = newPosition / contWidth
                }
            }
        }
    }

    // --------------------------------------
    // instantiate cpp models
    MuPdfModel {
        id: pdfModel
        mustProcessSignatures: true
        mustDeleteTmpFiles: true
        mustExtractText: true
    }

    FileTreeModel {
        id: fileTreeModel

        onIsDraftChanged: {
            if (fileTreeModel.isDraft)
                header.disableSignMode()
            else{
                header.enableSignMode()
            }
        }
    }

    ProfilesModel {
        id: profilesModel
    }

    RubberStampModel {
        id: rubberStampModel
    }

    SignaturesListModel {
        id: siglistModel
    }

    PrinterLauncher {
        id: printer
    }

    TagCreator {
        id: tagCreator
    }

    SurguchLauncher {
        id: launcher
    }

    SigCreatorWrapper {
        id: sigCreatorWrapper
    }

    WheelFilter {
        id: main_window_wheel_filter
    }

    SurguchTranslator {
        id: surguchTranslator
    }

    // --------------------------------------
    //  connect the events
    Component.onCompleted: {
        // set themes
        StyleSheet.state = themeStyle
        // connect basic events
        EventsConnect.initEvents()

        // mouse events rethrow for Alt-C10.
        EventFilterInstaller.installEventFilter(this, main_window_wheel_filter)
    }

    // ---------------------------------------------
    // helper dialogs
    StampEditor {
        id: stampEditor
    }
    RubberStampEditor {
        id: rubberStampEditor
    }
    AboutDialog {
        id: aboutDialog
    }

    TreeSignResultDialog {
        id: treeSignResultDialog
    }

    // Info dialog in center of window
    InfoDialog {
        id: infoDialog
    }

    // error message dialog
    ErrorMessage {
        id: errorMessageDialog
    }

    UnsavedChangesDialog {
        id: unsavedFileDialog
    }

    DisappearingHint {
        id: disappearingHint

        anchors.horizontalCenter: parent.horizontalCenter
        y: 20
    }

    onShowTypeChanged: {
        if (showType === Main.ShowType.Empty) {
            header.disableSignMode()
        }
    }

    onWidthChanged: {
        if (width <= 900) {
            StyleSheet.window_size_x = "small_width"
        } else {
            StyleSheet.window_size_x = "normal"
        }
    }

    onHeightChanged: {
        if (height <= 770) {
            StyleSheet.window_size_y = "small_height"
        } else {
            StyleSheet.window_size_y = "normal"
        }
    }
}
