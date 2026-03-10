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
                visible: pdfListView.source != ""
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
            else
                header.enableSignMode()
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
        // enable sign button
        fileTreeView.enableSignButton.connect(header.enableSignMode)
        // disable sign button
        fileTreeView.disableSignButton.connect(header.disableSignMode)
        // clean windows after signing tree
        fileTreeView.cleanWindow.connect(function () {
            root_window.showType = Main.ShowType.Empty
            fileDropArea.width = width / 2
            fileDropArea.enabled = true
            pdfDropArea.width = width / 2
            pdfDropArea.enabled = true
        })
        // attmept to sign files in tree
        header.signTree.connect(fileTreeView.gatherParamsTree)
        // show data about MrpaList
        fileTreeView.showMrpaList.connect(rightSideBar.showMrpaList)
        // update page count in header
        pdfListView.pagesCountChanged.connect(headerSubBar.changePageCount)
        // update curr page in header
        pdfListView.currPageChanged.connect(headerSubBar.changedCurrPage)
        pdfListView.currPageChanged.connect(leftSideBar.scrollToPage)
        pdfListView.pageWidthUpdate.connect(horizontalScroll.setContentWidth)
        // scroll to page
        headerSubBar.scrollToPage.connect(pdfListView.scrollToPage)
        leftSideBar.pageClick.connect(pdfListView.scrollToPage)
        // show signature info
        leftSideBar.showSigData.connect(rightSideBar.showData)
        rightSideBar.showSigData.connect(rightSideBar.showData)
        rightSideBar.showMrpaData.connect(rightSideBar.showMrpa)
        // update zoom value in header
        pdfListView.zoomFactorUpdate.connect(headerSubBar.updateZoomValue)
        // update horizontal scroll position after flick
        pdfListView.hScrollUpdate.connect(horizontalScroll.updateScrollPosition)
        // zoomIn render
        headerSubBar.zoomInClicked.connect(pdfListView.zoomIn)
        // zoomOut render
        headerSubBar.zoomOutClicked.connect(pdfListView.zoomOut)
        // zoom dropbox preset value was selected
        headerSubBar.zoomSelected.connect(pdfListView.setZoom)
        // rotate
        headerSubBar.rotateClockwise.connect(pdfListView.rotateClockWise)
        headerSubBar.rotateCounterClockWise.connect(
                    pdfListView.rotateCounterClockWise)
        // enable/disable zoom
        pdfListView.maxZoomReached.connect(headerSubBar.disableZoom)
        pdfListView.canZoom.connect(headerSubBar.enableZoom)
        pdfListView.minZoomReached.connect(headerSubBar.disableZoomOut)
        pdfListView.canZoomOut.connect(headerSubBar.enableZoomOut)
        ///sync action history
        headerSubBar.undoAction.connect(pdfListView.undo)
        headerSubBar.redoAction.connect(pdfListView.redo)
        pdfListView.updateHistory.connect(headerSubBar.updateHistory)
        // toggle from preview to certs in left sidebar
        headerSubBar.showPreviews.connect(leftSideBar.showPreviews)
        headerSubBar.showCerts.connect(leftSideBar.showCerts)
        // screen DPI changed
        pdfModel.screenDpiChanged.connect(pdfListView.redrawAndPreservePosition)
        //enable buttons for stamps
        pdfListView.quitSignMode.connect(header.quitSignMode)
        pdfListView.disableTagMode.connect(headerSubBar.disableTagMode)
        // search
        headerSubBar.searchDialog.searchRequired.connect(pdfModel.performSearch)
        pdfModel.searchCompleted.connect(pdfListView.searchCompleted)
        pdfModel.searchCompleted.connect(
                    headerSubBar.searchDialog.searchCompleted)
        headerSubBar.searchDialog.jumpToNeedle.connect(pdfModel.jumpToNeedle)
        pdfModel.jumpToNeedleCompleted.connect(pdfListView.jumpToNeedle)
        // sign the document
        pdfListView.stampLocationSelected.connect(header.disableSignMode)
        //  save signatures count in left sidebar
        pdfModel.signaturesCounted.connect(leftSideBar.setSigCount)
        //  save signatures count in right sidebar
        fileTreeModel.updateSigCount.connect(rightSideBar.setSigCount)
        // call SignaturesListModel to update the signatures list and validate all signatures
        pdfModel.signaturesFound.connect(siglistModel.updateSigList)
        fileTreeModel.signatureReady.connect(
                    siglistModel.saveValidationResultBatch)
        // add rubber stamp to document
        pdfListView.tagPlaced.connect(headerSubBar.enableTagButton)
        // sync pdflistpreview with changed source of pdflistview
        pdfListView.updateLSB.connect(leftSideBar.updateSource)
        // open file error
        pdfModel.errorOpenFile.connect(function (err_string) {
            console.warn("ERROR OPEN:" + err_string)
            errorMessageDialog.addError(err_string)
            errorMessageDialog.show()
            pdfListView.source = ""
            leftSideBar.source = ""
            Qt.callLater(function () {
                root_window.showType = Main.ShowType.Empty
                fileDropArea.width = width / 2
                fileDropArea.enabled = true
                pdfDropArea.width = width / 2
                pdfDropArea.enabled = true
            })
        })
        // file common status alerts
        siglistModel.commonDocStatus.connect(function (status) {
            console.warn("status:" + status)
            switch (status) {
            case "kDocCanBeRecovered":
                errorMessageDialog.addError(
                            qsTr("The document was changed after signing, but can be restored"))
                break
            case "kDocCantBeTrusted":
                errorMessageDialog.addError(
                            qsTr("The document can't be trusted because none of signatures covers the whole document.﻿"))
                break
            case "kDocCanBeRecoveredButSuspicious":
                errorMessageDialog.addError(
                            qsTr("The document was changed after signing.Some of signatures does not cover the whole document, should be considered it suspicious.﻿﻿"))
                break
            case "kDocSuspiciousPrevious":
                errorMessageDialog.addError(
                            qsTr("Some of signatures does not cover the whole document, should be considered it suspicious.﻿﻿"))
                break
            }
            errorMessageDialog.show()
        })
        // open error window if file singing went wrong
        fileTreeView.errorOnSign.connect(function (err) {
            switch (err) {
            case "INVALID_PARAMETERS":
                errorMessageDialog.text = qsTr("Invalid parameters")
                errorMessageDialog.show()
                treeSignResultDialog.close()
                break
            case "INVALID_DESTINATION":
                errorMessageDialog.text = qsTr("Invalid destination path")
                errorMessageDialog.show()
                treeSignResultDialog.close()
                break
            case "SIGN_ALL_FILES_FAILED":
                errorMessageDialog.text = qsTr(
                            "Failed to sign files: check the certificate in the profile and CryptoPro (availability and expiration date).")
                errorMessageDialog.show()
                treeSignResultDialog.close()
                break
            case "CREATE_ZIP_FAILED":
                errorMessageDialog.text = qsTr("Failed to create archive")
                errorMessageDialog.show()
                treeSignResultDialog.close()
                break
            case "COPY_SRC_FILES_FAILED":
                errorMessageDialog.text = qsTr(
                            "You trying to create files which already exist")
                errorMessageDialog.show()
                treeSignResultDialog.close()
                break
            case "COPY_SRC_MRPA_FILES_FAILED":
                errorMessageDialog.text = qsTr(
                            "You trying to create files which already exist")
                errorMessageDialog.show()
                treeSignResultDialog.close()
                break
            case "SOME_FILES_WHERE_RENAMED":
                errorMessageDialog.text = qsTr("Some files were renamed")
                errorMessageDialog.show()
                break
            }
        })
        // open the recovered file
        siglistModel.fileRecovered.connect(function (dest) {
            rightSideBar.showState = RightSideBar.ShowState.Invisible
            pdfListView.openTmpFile(dest)
            leftSideBar.source = dest
        })
        // validation failed
        siglistModel.validationFailedForSignature.connect(function (index) {

            errorMessageDialog.addError(
                        qsTr("Validation failed for signature number") + " " + index)
            errorMessageDialog.show()
        })
        // no cryptoPro error
        if (profilesModel.errStatus) {
            if (profilesModel.errString === "ERR_NO_CSP_LIB") {
                disappearingHint.showHint(
                            qsTr("CryptoPro CSP 5.0 R3 not found, please check if installed"),
                            1500)
            } else if (profilesModel.errString === "ERR_GET_CERTS") {
                errorMessageDialog.addError(
                            qsTr("Failed getting the user's certificates list"))
                errorMessageDialog.show()
            } else {
                errorMessageDialog.addError("err: " + profilesModel.errString)
                errorMessageDialog.show()
            }
        }
        ;
        // close window
        root_window.closing.connect(function (close_event) {
            if (pdfListView.sourceIsTmp) {
                close_event.accepted = false
                unsavedFileDialog.quit_after = true
                unsavedFileDialog.open()
            }
        })
        unsavedFileDialog.saveWithQuit.connect(header.launchSaveFileWithQuit)
        headerSubBar.quitApp.connect(function () {
            close()
        })
        // go into file mode after pdf
        unsavedFileDialog.openTreeDialog.connect(header.openTreeDialog)
        // invalid pdf
        pdfModel.docWasRepaired.connect(function () {
             // disable signing for damaged document
            header.disableSignMode()
            errorMessageDialog.addError(
                        qsTr("Errors were found in the document when it was opened. The document may be displayed incorrectly."))
            errorMessageDialog.show()


        })
        // update AimSize when profile was edited
        rightSideBar.profileSaved.connect(pdfListView.forceAimResize)
        // update AimSize when stamp was edited
        stampEditor.stampSaved.connect(pdfListView.forceAimResize)
        EventFilterInstaller.installEventFilter(this, main_window_wheel_filter)
        // open file tree on start
        if (openFiles.length !== 0) {
            pdfDropArea.enabled = false
            fileDropArea.width = width
            showType = Main.ShowType.Files
            fileTreeModel.addNode(openFiles)
        } else // open document on start
            if (openOnStart !== "") {
                header.enableSignMode()
                pdfListView.openFile(openOnStart )
                leftSideBar.source = openOnStart
                rightSideBar.showState = RightSideBar.ShowState.Invisible
                showType = Main.ShowType.Pdf
            }
    }

    // ---------------------------------------------
    // helper dialogs
    StampEditor {
        id: stampEditor
    }
    RubberStampEditor {
        id: rubberStampEditor
    }
    InfoDialog {
        id: appInfoDialog
    }

    TreeSignResultDialog {
        id: treeSignResultDialog
    }

    // Info dialog in center of window
    Dialog {
        id: infoDialog

        width: root_window.width - 200
        height: root_window.height - 100
        modal: true
        standardButtons: Dialog.Ok
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        topPadding: StyleSheet.defaultPaddingV
        bottomPadding: StyleSheet.defaultPaddingV
        leftPadding: StyleSheet.defaultPaddingH
        rightPadding: StyleSheet.defaultPaddingH
        topMargin: StyleSheet.defaultMarginV
        bottomMargin: StyleSheet.defaultMarginV
        leftMargin: StyleSheet.defaultMarginH
        rightMargin: StyleSheet.defaultMarginH

        Loader {
            id: infoDialogContentContainer
            width: parent.width
            height: parent.height
        }
    }

    Dialog {
        id: errorMessageDialog

        function addError(err_str) {
            if (text != "") {
                errorMessageDialog.text += "\n"
            }
            text += err_str
        }

        function show() {
            if (!opened && text != "") {
                open()
            }
        }

        modal: true
        title: qsTr("Error")
        standardButtons: Dialog.Ok
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
        x: (parent.width - width) / 2
        y: (parent.height - height) / 2
        topPadding: StyleSheet.defaultPaddingV
        bottomPadding: StyleSheet.defaultPaddingV
        leftPadding: StyleSheet.defaultPaddingH
        rightPadding: StyleSheet.defaultPaddingH
        topMargin: StyleSheet.defaultMarginV
        bottomMargin: StyleSheet.defaultMarginV
        leftMargin: StyleSheet.defaultMarginH
        rightMargin: StyleSheet.defaultMarginH
        readonly property int maxWidth: root_window.width - leftMargin - rightMargin

        Component.onCompleted: {
            if (width > maxWidth)
                width = maxWidth
        }

        property string text: ""

        contentItem: Text {
            id: message_text
            color: StyleSheet.font_color_extra

            text: errorMessageDialog.text
            font.family: "Noto Sans"
            wrapMode: Text.WordWrap
            maximumLineCount: 10
        }

        onClosed: {
            text = ""
        }
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
