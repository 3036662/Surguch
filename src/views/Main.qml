import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
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
                                          fileDropArea.enabled = false
                                          pdfDropArea.width = parent.width
                                      }
                                      if (showType === Main.ShowType.Files) {
                                          fileDropArea.enabled = false
                                          pdfDropArea.width = parent.width
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
                       console.warn(
                           "something dropped in pdf area " + drop.urls)
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
                       console.warn(
                           "something dropped in file area" + drop.urls)
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
            visible: showType === Main.ShowType.Empty
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

            Layout.preferredWidth: root_window.width - 300
            Layout.maximumWidth: root_window.width - 300
            Layout.minimumWidth: root_window.width - 300
            visible: showType === Main.ShowType.Files
        }

        RightSideBar {
            id: rightSideBar
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
            errorMessageDialog.text = err_string
            errorMessageDialog.open()
            pdfListView.source = ""
            leftSideBar.source = ""
        })
        // file common status alerts
        siglistModel.commonDocStatus.connect(function (status) {
            //console.warn("status:"+status)
            switch (status) {
            case "kDocCanBeRecovered":
                errorMessageDialog.text = qsTr(
                            "The document was changed after signing, but can be restored")
                errorMessageDialog.open()
                break
            case "kDocCantBeTrusted":
                errorMessageDialog.text = qsTr(
                            "The document can't be trusted because none of signatures covers the whole document.﻿")
                errorMessageDialog.open()
                break
            case "kDocCanBeRecoveredButSuspicious":
                errorMessageDialog.text = qsTr(
                            "The document was changed after signing.Some of signatures does not cover the whole document, should be considered it suspicious.﻿﻿")
                errorMessageDialog.open()
                break
            case "kDocSuspiciousPrevious":
                errorMessageDialog.text = qsTr(
                            "Some of signatures does not cover the whole document, should be considered it suspicious.﻿﻿")
                errorMessageDialog.open()
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
            errorMessageDialog.text = qsTr(
                        "Validation failed for signature number") + " " + index
            errorMessageDialog.open()
        })
        // open document on start
        if (openOnStart !== "") {
            pdfListView.openFile(openOnStart)
            header.enableSignMode()
            leftSideBar.source = openOnStart
            rightSideBar.showState = RightSideBar.ShowState.Invisible
            showType = Main.ShowType.Pdf
        }

        // no cryptoPro error
        if (profilesModel.errStatus) {
            if (profilesModel.errString === "ERR_NO_CSP_LIB") {
                // errorMessageDialog.text = qsTr(
                //             "CryptoPro CSP 5.0 R3 not found, please check if installed")
                disappearingHint.showHint(
                            qsTr("CryptoPro CSP 5.0 R3 not found, please check if installed"),
                            1500)
            } else if (profilesModel.errString === "ERR_GET_CERTS") {
                errorMessageDialog.text = qsTr(
                            "Failed getting the user's certificates list")
                errorMessageDialog.open()
            } else {
                errorMessageDialog.text = "err: " + profilesModel.errString
                errorMessageDialog.open()
            }
        }
        ;
        // close window
        root_window.closing.connect(function (close_event) {
            if (pdfListView.sourceIsTmp) {
                close_event.accepted = false
                unsavedFileDialog.open()
            }
        })
        unsavedFileDialog.saveWithQuit.connect(header.launchSaveFileWithQuit)
        // invalid pdf
        pdfModel.docWasRepaired.connect(function () {
            errorMessageDialog.text = qsTr(
                        "Errors were found in the document when it was opened. The document may be displayed incorrectly.")
            errorMessageDialog.open()
            // disable signing for damaged document
            header.disableSignMode()
        })
        // update AimSize when profile was edited
        rightSideBar.profileSaved.connect(pdfListView.forceAimResize)
        // update AimSize when stamp was edited
        stampEditor.stampSaved.connect(pdfListView.forceAimResize)

        // set themes
        StyleSheet.state = themeStyle
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

        // Handle dialog closing
        onAccepted: {

            //   console.log("Dialog closed")
        }
    }

    MessageDialog {
        id: errorMessageDialog
        buttons: MessageDialog.Ok
        title: qsTr("Error")

        onAccepted: {

            //console.log("Error message dialog closed.")
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
