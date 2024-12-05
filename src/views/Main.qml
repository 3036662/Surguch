import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts
import alt.pdfcsp.pdfModel
import alt.pdfcsp.signatureCreator
import alt.pdfcsp.profilesModel
import alt.pdfcsp.signaturesListModel
import alt.pdfcsp.printerLauncher

ApplicationWindow {
    id: root_window

    width: 1000
    height: 480
    visible: true
    title: qsTr("Surguch")


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
            }
        }
    }


    // --------------------------------------
    // body

    RowLayout {
        anchors.fill: parent

        LeftSideBar {
            id: leftSideBar
        }
        PdfListView {
            id: pdfListView
            Layout.preferredWidth: root_window.width-500;
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
        height: 30
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
                console.warn(position + size)
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
    }

    ProfilesModel {
        id: profilesModel
    }

    SignaturesListModel {
        id: siglistModel
    }

    PrinterLauncher{
        id:printer
    }

    SignatureCreator {
        id: sigCreator

        // common function to gather parameters used in resizeAim and signDoc
        function gatherParams(location_data){
            let curr_profile = JSON.parse(header.getCurrentProfileValue());
            let cert_array = JSON.parse(profilesModel.getUserCertsJSON())
            // console.warn(JSON.stringify(rightSideBar.edit_profile.cert_array));
            let cert_index = cert_array.findIndex(cert => {
                                                      return curr_profile.cert_serial
                                                      === cert.serial
                                                  })
            if (cert_index === -1) {
                errorMessageDialog.text=qsTr("Certificate not found, looks like it was deleted.﻿");
                errorMessageDialog.open();
                throw new Error('Certificate data not found');
            }
            // gather all information needed to create a signature visual representation
            let params = {
                page_index: location_data.page_index,
                page_width: location_data.page_width,
                page_height: location_data.page_height,
                stamp_x: location_data.stamp_x,
                stamp_y: location_data.stamp_y,
                stamp_width: location_data.stamp_width,
                stamp_height: location_data.stamp_height,
                logo_path: curr_profile.logo_path,
                config_path: profilesModel.getConfigPath(),
                cert_serial: curr_profile.cert_serial,
                cert_serial_prefix: qsTr("Certificate: "),
                cert_subject: cert_array[cert_index].subject_common_name,
                cert_subject_prefix: qsTr("Subject: "),
                cert_time_validity: qsTr("Vaildity: ")+cert_array[cert_index].not_before_readable + qsTr(
                    " till ") + cert_array[cert_index].not_after_readable,
                stamp_title:qsTr("THE DOCUMENT IS SIGNED WITH AN ELECTRONIC SIGNATURE"),
                stamp_type: curr_profile.stamp_type,
                cades_type: curr_profile.CADES_format,
                tsp_url:curr_profile.tsp_url,
                file_to_sign_path: pdfModel.getSource()
            }
            //console.warn(JSON.stringify(params))
            return params;
        }

        // estimate the resulting stamp size
        function resizeAim(location_data){
            try{
                let params=gatherParams(lockation_data);
                sigCreator.estimateStampResizeFactor(params);
            } catch(e){
                console.warn(e);
            }
        }

        // sign the document
        function signDoc(location_data) {
            try {
                let params=gatherParams(location_data);
                sigCreator.createSignature(params)
            } catch (e) {
                console.warn(e)
            }
        }

        // handle the result of signDoc function
        function handleSigResult(result){
            console.warn(result.status);
            if (!result.status){
              if (result.err_string==="CERT_EXPIRED"){
                errorMessageDialog.text=qsTr("Your certificate is expired.");
              } else if (result.err_string==="MAYBE_TSP_URL_INVALID"){
                errorMessageDialog.text=qsTr("Common error. It looks like the TSP URL is not valid.");
              } else{
                  errorMessageDialog.text=qsTr("Common error");
              }
              errorMessageDialog.open();
            }
            // if successfully signed
            else{
             if (result.tmp_file_path!==undefined){
                 // open with openTmpFile, to be deleted later
                 pdfListView.openTmpFile(result.tmp_file_path)
                 leftSideBar.source = result.tmp_file_path
                 rightSideBar.showState = RightSideBar.ShowState.Invisible
             }
            }
            header.enableSignMode();
        }
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
        // toggle from preview to certs in left sidebat
        headerSubBar.showPreviews.connect(leftSideBar.showPreviews)
        headerSubBar.showCerts.connect(leftSideBar.showCerts)
        // sign the document
        pdfListView.stampLocationSelected.connect(header.disableSignMode);
        pdfListView.stampLocationSelected.connect(sigCreator.signDoc)
        // stamp size estimated
        sigCreator.stampSizeEstimated.connect(pdfListView.updateStampResizeFactor);
        // sign creation finished
        sigCreator.signCompleted.connect(sigCreator.handleSigResult)
        //  save signatures count in left sidebar
        pdfModel.signaturesCounted.connect(leftSideBar.setSigCount)
        // call SignaturesListModel to update the signatures list and validate all signatures
        pdfModel.signaturesFound.connect(siglistModel.updateSigList)
        // file common status alerts
        siglistModel.commonDocStatus.connect(function(status){
            switch(status){
            case DocStatusEnum.CommonDocCoverageStatus.kDocCanBeRecovered:
                errorMessageDialog.text=qsTr("The document was changed after signing, but can be restored");
                errorMessageDialog.open();
                break;
            case DocStatusEnum.kDocCantBeTrusted:
                errorMessageDialog.text=qsTr("The document can't be trusted because none of signatures covers the whole document.﻿");
                errorMessageDialog.open();
                break;

            case DocStatusEnum.kDocCanBeRecoveredButSuspicious:
                errorMessageDialog.text=qsTr("The document was changed after signing.Some of signatures does not cover the whole document, should be considered it suspicious.﻿﻿");
                errorMessageDialog.open();
                break;
            case DocStatusEnum.kDocSuspiciousPrevious:
                errorMessageDialog.text=qsTr("Some of signatures does not cover the whole document, should be considered it suspicious.﻿﻿");
                errorMessageDialog.open();
                break;
            }
        });
        // open the recovered file
        siglistModel.fileRecovered.connect(function(dest){
            rightSideBar.showState=RightSideBar.ShowState.Invisible
            pdfListView.openTmpFile(dest);
            leftSideBar.source = dest;
        });
        // validation failed
        siglistModel.validationFailedForSignature.connect(function(index){
            errorMessageDialog.text=qsTr("Validation failed for signature number")+" "+index;
            errorMessageDialog.open();
        });
        // open document on strart
        if (openOnStart!==""){
            pdfListView.openFile(openOnStart)
            leftSideBar.source = openOnStart
            rightSideBar.showState = RightSideBar.ShowState.Invisible
        }
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

        Loader {
            id: infoDialogContentContainer
            width: parent.width
            height: parent.height
        }

        // Handle dialog closing
        onAccepted: {
            console.log("Dialog closed")
        }
    }

    MessageDialog {
            id: errorMessageDialog
            buttons: MessageDialog.Ok
            title: "Error"
            onAccepted: {
                console.log("Error message dialog closed.")
            }
    }
}
