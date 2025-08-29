import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts
import alt.pdfcsp.pdfModel
import alt.pdfcsp.signatureCreator
import alt.pdfcsp.tagCreator
import alt.pdfcsp.profilesModel
import alt.pdfcsp.rubberStampModel
import alt.pdfcsp.signaturesListModel
import alt.pdfcsp.printerLauncher
import StyleSheet
import alt.pdfcsp.eventFilterInstaller
import alt.pdfcsp.wheelFilter

ApplicationWindow {
    id: root_window

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
            }
            HeaderSubBar {
                id: headerSubBar
                visible: pdfListView.source != ""

                function placeTagStamp(rubber_stamp_data) {//let tag_data = rubber_stamp_data
                    //console.warn("mainqml" + JSON.stringify(tag_data))
                    //pdfModel.placeRubberStamp(tag_data)
                }
            }
        }
    }

    // --------------------------------------
    // body
    RowLayout {
        anchors.fill: parent
        spacing: 0

        LeftSideBar {
            id: leftSideBar
        }
        PdfListView {
            id: pdfListView
            Layout.preferredWidth: root_window.width - 500
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
    // instantinate cpp models
    MuPdfModel {
        id: pdfModel
        mustProcessSignatures: true
        mustDeleteTmpFiles: true
        mustExtractText: true
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

    SigCreatorWrapper{
        id: sigCreatorWrapper
    }

    WheelFilter {
        id: main_window_wheel_filter
    }

    // --------------------------------------
    //  connect the events
    Component.onCompleted: {
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
        // toggle from preview to certs in left sidebat
        headerSubBar.showPreviews.connect(leftSideBar.showPreviews)
        headerSubBar.showCerts.connect(leftSideBar.showCerts)
        // screen DPI changed
        pdfModel.screenDpiChanged.connect(pdfListView.redrawAndPreservePosion)
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
        // call SignaturesListModel to update the signatures list and validate all signatures
        pdfModel.signaturesFound.connect(siglistModel.updateSigList)
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
        // open document on strart
        if (openOnStart !== "") {
            pdfListView.openFile(openOnStart)
            header.enableSignMode()
            leftSideBar.source = openOnStart
            rightSideBar.showState = RightSideBar.ShowState.Invisible
        }

        // no cryptoPro error
        if (profilesModel.errStatus) {
            if (profilesModel.errString === "ERR_NO_CSP_LIB") {
                errorMessageDialog.text = qsTr(
                            "CryptoPro CSP 5.0 R3 not found, please check if installed")
            } else if (profilesModel.errString === "ERR_GET_CERTS") {
                errorMessageDialog.text = qsTr(
                            "Failed getting the user's certificates list")
            } else {
                errorMessageDialog.text = "err: " + profilesModel.errString
            }
            errorMessageDialog.open()
        }
        ;
        // close window
        root_window.closing.connect(function (close_event) {
            if (pdfListView.sourceIsTmp) {
                close_event.accepted = false
                undsavedFileDialog.open()
            }
        })
        undsavedFileDialog.saveWithQuit.connect(header.launchSaveFileWithQuit)
        // invalid pdf
        pdfModel.docWasRepaired.connect(function () {
            errorMessageDialog.text = qsTr(
                        "Errors were found in the document when it was opened. The document may be displayed incorrectly.")
            errorMessageDialog.open()
            // disable signing for damaged document
            header.disableSignMode()
        })
        // update AimSize when profile was edited
        rightSideBar.profileSaved.connect(pdfListView.forceAimResize);

        // set themes
        StyleSheet.state = themeStyle
        EventFilterInstaller.installEventFilter(this, main_window_wheel_filter)
    }

    // ---------------------------------------------
    // helper dialogs

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
        id: undsavedFileDialog
    }

    onWidthChanged: {
        if (width <= 900) {
            StyleSheet.window_size_x = "small_width"
        } else {
            StyleSheet.window_size_x = "normal"
        }
    }

    onHeightChanged: {
        if (height <= 600) {
            StyleSheet.window_size_y = "small_height"
        } else {
            StyleSheet.window_size_y = "normal"
        }
    }
}
