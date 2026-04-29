// Connect all the events
//
// root - main window
function initEvents() {
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

    // attempt to sign files in tree
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

    //sync action history
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
            errorMessageDialog.addError(qsTr("Invalid parameters"))
            errorMessageDialog.show()
            treeSignResultDialog.close()
            break
        case "INVALID_DESTINATION":
            errorMessageDialog.addError(qsTr("Invalid destination path"))
            errorMessageDialog.show()
            treeSignResultDialog.close()
            break
        case "SIGN_ALL_FILES_FAILED":
            errorMessageDialog.addError(qsTr(
                        "Failed to sign files: check the certificate in the profile and CryptoPro (availability and expiration date)."))
            errorMessageDialog.show()
            treeSignResultDialog.close()
            break
        case "CREATE_ZIP_FAILED":
            errorMessageDialog.addError(qsTr("Failed to create archive"))
            errorMessageDialog.show()
            treeSignResultDialog.close()
            break
        case "COPY_SRC_FILES_FAILED":
            errorMessageDialog.addError(qsTr(
                        "You trying to create files which already exist"))
            errorMessageDialog.show()
            treeSignResultDialog.close()
            break
        case "COPY_SRC_MRPA_FILES_FAILED":
            errorMessageDialog.addError(qsTr(
                        "You trying to create files which already exist"))
            errorMessageDialog.show()
            treeSignResultDialog.close()
            break
        case "SOME_FILES_WHERE_RENAMED":
            errorMessageDialog.addError(qsTr("Some files were renamed"))
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

    // go into file mode after PDF
    unsavedFileDialog.openTreeDialog.connect(header.openTreeDialog)

    // invalid PDF
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

    // open file tree on start
    if (openFiles.length !== 0) {
        pdfDropArea.enabled = false
        fileDropArea.width = width
        showType = Main.ShowType.Files
        fileTreeModel.addNode(openFiles)
    } else // open document on start
        if (openOnStart !== "") {            
            header.enableSignMode()
            pdfListView.openFile(openOnStart)
            leftSideBar.source = openOnStart
            rightSideBar.showState = RightSideBar.ShowState.Invisible
            showType = Main.ShowType.Pdf
        }
}
