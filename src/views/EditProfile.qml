import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt.labs.platform
import QtCore
import StyleSheet

import "info_panel_components" as InfoPanelComponents
import "EditProfile.js" as EpJS

Flickable {
    id: root

    property var cert_data_raw
    property var stamps_data_raw
    property string profile_data
    property var profile_json
    property var cert_array
    property var cert_combo_model
    property var stamps_array
    property var stamps_combo_model
    property var profiles_model
    property int profile_id: -1

    signal profileSaved
    signal closeClicked

    // fill the form from profile_data JSON string
    function updateProfileForm() {
        EpJS.updateProfileForm()
    }

    width: parent.width
    height: parent.height
    contentHeight: profileColumn.height + 30
    leftMargin: 10
    rightMargin: 10
    topMargin: 10

    InfoPanelComponents.RSBCloseButton {
        onClicked: {
            closeClicked()
        }
    }

    Column {
        id: profileColumn
        width: parent.width

        Text {
            text: qsTr("Profile settings")
            font.weight: Font.DemiBold
            topPadding: 10
            bottomPadding: 10
            font.family: "Noto Sans"
            color: StyleSheet.font_color_extra
        }

        InfoPanelComponents.TextPair {
            visible: false
            id: profileIdTextPair
            keyText: qsTr("Profile id")
        }

        // profile name
        Text {
            text: qsTr("Profile name")
            bottomPadding: 5
            font.family: "Noto Sans"
            color: StyleSheet.font_color_extra
        }

        InfoPanelComponents.RSBTextArea {
            id: profileName
            placeholderText: qsTr("Enter profile name")
            color: StyleSheet.font_color_extra

            onTextChanged: {
                const cursorPos = cursorPosition
                const cleanedText = profileName.text.replace(/\s/g, '')
                if (cleanedText !== text) {
                    text = cleanedText
                    cursorPosition = Math.min(cursorPos - 1, text.length)
                }
            }
        }

        // certificate choice
        Text {
            text: qsTr("Certificate")
            topPadding: 10
            bottomPadding: 10
            font.family: "Noto Sans"
            color: StyleSheet.font_color_extra
        }

        InfoPanelComponents.RSBComboSelect {
            id: selectCertificateCombo

            property string displayTextDefault: qsTr("Select the certificate")

            model: root.cert_combo_model
            textRole: "title"
            valueRole: "serial"
            displayText: displayTextDefault

            property real shake: 0

            x: Math.sin(shake * Math.PI * 10) * 6

            NumberAnimation {
                id: shakeAnimation
                target: selectCertificateCombo
                property: "shake"
                from: 0
                to: 1
                duration: 200
            }
        }

        InfoPanelComponents.RightSBHorizontalDelimiter {
            width: parent.width
            topPadding: 7
            bottomPadding: 7
        }

        // use by default switch
        InfoPanelComponents.RSBSwitch {
            id: useAsDefaultProfileSwitch
            topPadding: 5
            bottomPadding: 5
            text: qsTr("Use this profile by default")
        }

        InfoPanelComponents.RightSBHorizontalDelimiter {
            width: parent.width
            topPadding: 10
            bottomPadding: 10
        }

        Text {
            text: qsTr("Signature")
            font.weight: Font.DemiBold
            topPadding: 10
            bottomPadding: 10
            font.family: "Noto Sans"
            color: StyleSheet.font_color_extra
        }

        // Cades format
        Text {
            topPadding: 5
            text: qsTr("Cades type")
            bottomPadding: 5
            font.family: "Noto Sans"
            color: StyleSheet.font_color_extra
        }

        InfoPanelComponents.RSBComboSelect {
            id: selectCadesFormatCombo
            model: [{
                    "title": "CADES_BES"
                }, {
                    "title": "CADES_T"
                }, {
                    "title": "CADES_XLT1"
                }]
            textRole: "title"
            valueRole: "title"
            property string displayTextDefault: qsTr("Select Cades format")
            currentIndex: 2
            item_selected: true
        }

        // tsp url
        Column {
            id: tspUrlWrapper
            width: parent.width
            visible: selectCadesFormatCombo.item_selected
                     && (selectCadesFormatCombo.currentValue === "CADES_T"
                         || selectCadesFormatCombo.currentValue === "CADES_XLT1")
            Text {
                topPadding: 10
                text: qsTr("TSP server URL")
                bottomPadding: 5
                font.family: "Noto Sans"
                color: StyleSheet.font_color_extra
            }

            InfoPanelComponents.RSBTextArea {
                id: tspUrlEdit
                placeholderText: qsTr("Enter TSP service url")
                color: StyleSheet.font_color_extra
                inputMethodHints: Qt.ImhUrlCharactersOnly
                property bool valid_url: false

                onTextChanged: {
                    try {
                        new URL(tspUrlEdit.text)
                        tspUrlEdit.color = "green"
                        tspUrlEdit.valid_url = true
                    } catch (err) {
                        tspUrlEdit.color = "red"
                        tspUrlEdit.valid_url = false
                    }
                }
            }
        }

        InfoPanelComponents.RightSBHorizontalDelimiter {
            width: parent.width
            topPadding: 10
            bottomPadding: 10
        }

        Text {
            text: qsTr("Stamp")
            font.weight: Font.DemiBold
            topPadding: 10
            bottomPadding: 10
            font.family: "Noto Sans"
            color: StyleSheet.font_color_extra
        }

        // stamp settings
        RowLayout {
            width: parent.width

            Text {
                topPadding: 10
                text: qsTr("Stamp appearance")
                bottomPadding: 5
                font.family: "Noto Sans"
                color: StyleSheet.font_color_extra
            }

            Rectangle {
                Layout.fillWidth: true
            }

            ToolButton {
                id: editButton
                flat: true
                display: AbstractButton.IconOnly
                icon.width: 15
                icon.height: 15
                leftPadding: 5
                rightPadding: 5
                topPadding: 5
                bottomPadding: 5
                font.family: "Noto Sans"
                icon.source: StyleSheet.wrench_icon
                enabled: selectStampTypeCombo.currentText !== "ГОСТ"

                onClicked: EpJS.editButtonClicked();

            }
        }

        RowLayout {
            width: parent.width

            MouseArea {
                Layout.fillWidth: true
                height: selectStampTypeCombo.height
                enabled: !selectStampTypeCombo.enabled
                onClicked: {
                    root.contentY = 0
                    shakeAnimation.start()
                }

                InfoPanelComponents.RSBComboSelect {
                    id: selectStampTypeCombo
                    Layout.fillWidth: true
                    model: root.stamps_combo_model
                    textRole: "title"
                    valueRole: "value"
                    enabled: selectCertificateCombo.item_selected
                             && selectCadesFormatCombo.item_selected
                    property string displayTextDefault: qsTr(
                                                            "Select stamp type")
                    currentIndex: 0
                    item_selected: true

                    onActivated: EpJS.selectStampTypeComboActivated()
                }
            }
        }

        Connections {
            target: profilesModel

            // when model has successfully saved the stamp
            function onStampsSaved(val) {
               EpJS.stamptSaved();
            }

            function onStampDeleted(title) {
                EpJS.stampDeleted();
            }
        }

        RowLayout {
            width: parent.width

            // select a logo
            Text {
                topPadding: 10
                text: qsTr("Company logo")
                bottomPadding: 5
                font.family: "Noto Sans"
                color: StyleSheet.font_color_extra
            }
            Rectangle {
                Layout.fillWidth: true
            }

            ToolButton {
                id: deleteLogo
                flat: true
                display: AbstractButton.IconOnly
                icon.width: 15
                icon.height: 15
                leftPadding: 5
                rightPadding: 5
                topPadding: 5
                bottomPadding: 5
                icon.source: StyleSheet.trash_icon

                onClicked: {
                    logoPath.text = ""
                }
            }
        }

        InfoPanelComponents.RSBTextArea {
            id: logoPath
            placeholderText: qsTr("Select a logo")
            color: StyleSheet.font_color_extra

            MouseArea {
                anchors.fill: parent

                onClicked: {
                    imgFileDialog.open()
                }
            }
        }

        // files signing settings
        InfoPanelComponents.RightSBHorizontalDelimiter {
            width: parent.width
            topPadding: 10
            bottomPadding: 10
        }

        Text {
            text: qsTr("File signing")
            font.weight: Font.DemiBold
            topPadding: 10
            bottomPadding: 10
            font.family: "Noto Sans"
            color: StyleSheet.font_color_extra
        }

        Text {
            topPadding: 5
            text: qsTr("Sign type")
            bottomPadding: 5
            font.family: "Noto Sans"
            color: StyleSheet.font_color_extra
        }

        InfoPanelComponents.RSBComboSelect {
            id: selectFileSignType
            model: [{
                    "title": qsTr("Detached")
                }, {
                    "title": qsTr("Attached")
                }]
            textRole: "title"
            valueRole: "title"
            currentIndex: 0
            item_selected: true
            property string displayTextDefault: qsTr("Select sign format")
        }

        Text {
            topPadding: 5
            text: qsTr("Certificate file encoding")
            bottomPadding: 5
            font.family: "Noto Sans"
            color: StyleSheet.font_color_extra
        }

        InfoPanelComponents.RSBComboSelect {
            id: selectFileEncodingType
            model: [{
                    "title": "DER"
                }, {
                    "title": "PEM"
                }]
            textRole: "title"
            valueRole: "title"
            currentIndex: 0
            item_selected: true
            property string displayTextDefault: qsTr("Select encoding type")
        }

        Text {
            topPadding: 5
            text: qsTr("Signature file extension")
            bottomPadding: 5
            font.family: "Noto Sans"
            color: StyleSheet.font_color_extra
        }

        InfoPanelComponents.RSBComboSelect {
            id: selectFileExtension
            model: [{
                    "title": ".sig"
                }, {
                    "title": ".sign"
                }, {
                    "title": ".sgn"
                }, {
                    "title": ".p7s"
                }, {
                    "title": ".bin"
                }]
            textRole: "title"
            valueRole: "title"
            currentIndex: 0
            item_selected: true
            property string displayTextDefault: qsTr("Select signature file extension")
        }

        Text {
            topPadding: 5
            text: qsTr("Create an archive after signing")
            bottomPadding: 5
            font.family: "Noto Sans"
            color: StyleSheet.font_color_extra
        }

        InfoPanelComponents.RSBComboSelect {
            id: selectArchive
            model: [{
                    "title": qsTr("Don't use")
                }, {
                    "title": qsTr("Common ZIP-file")
                }, {
                    "title": qsTr("Separate ZIP-file")
                }]
            textRole: "title"
            valueRole: "title"
            currentIndex: 1
            item_selected: true
            property string displayTextDefault: qsTr("Select archive after signing")
        }

        // Save button
        Item {
            width: parent.width
            height: 50

            Button {
                id: saveButton
                display: AbstractButton.TextBesideIcon
                icon.source: StyleSheet.save_icon
                icon.width: 20
                icon.height: 20
                width: logoPath.width
                //width: text.length
                //     < deleteProfileButton.text.length ? deleteProfileButton.width : 150
                text: qsTr("Save profile")
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottom: parent.bottom
                font.family: "Noto Sans"

                onClicked: EpJS.saveButtonClicked()
            }
        } // save profile end

        // delete profile
        Item {
            width: parent.width

            height: 50
            Button {
                id: deleteProfileButton
                display: AbstractButton.TextBesideIcon
                icon.source: StyleSheet.trash_icon
                icon.width: 20
                icon.height: 20
                text: qsTr("Delete profile")
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottom: parent.bottom
                width: logoPath.width
                font.family: "Noto Sans"

                onClicked: {
                    deleteProfileButton.enabled = false
                    if (profiles_model.deleteProfile(root.profile_id)) {
                        rightSideBar.showState = RightSideBar.ShowState.Invisible
                    }
                }
            }
        } // delete profile end
    }

    FileDialog {
        id: imgFileDialog
        fileMode: FileDialog.OpenFile
        nameFilters: ["Image files (*.png *.jpg *.jpeg *.bmp)"]
        options: FileDialog.ReadOnly
        folder: StandardPaths.writableLocation(StandardPaths.DocumentsLocation)
        onAccepted: {
            logoPath.text = currentFile
        }
    }

    onProfile_dataChanged: EpJS.updateProfileForm();

    onCert_data_rawChanged: EpJS.certDataRawChanged();

    onStamps_data_rawChanged: EpJS.stampDataRawChanged();

    onVisibleChanged: {
        if (visible) {
            saveButton.enabled = true
            deleteProfileButton.enabled = true
        }
    }
}
