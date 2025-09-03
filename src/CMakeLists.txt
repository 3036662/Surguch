find_package(Qt6 6.4 REQUIRED COMPONENTS Quick REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Svg REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Widgets REQUIRED)

if (NOT DEFINED TRANSLATION_DIR)
    set(TRANSLATION_DIR ${CMAKE_BINARY_DIR}/src/)
endif ()
message(TRANSLATION_DIR=${TRANSLATION_DIR})

set(CMAKE_AUTORCC ON)

add_compile_definitions(TRANSLATION_DIR="${TRANSLATION_DIR}")

#target_compile_options(surguch PRIVATE -fsanitize=address)
#target_link_libraries(surguch PRIVATE -fsanitize=address)
#target_compile_options(surguch PRIVATE -fsanitize=memory)
#target_link_libraries(surguch PRIVATE -fsanitize=memory)


IF (NOT DEFINED MUPDF_INCLUDE_DIR)
    SET(MUPDF_INCLUDE_DIR /usr/include/)
ENDIF ()

IF (NOT DEFINED MUPDF_LINK_PATH)
    SET(MUPDF_LINK_PATH mupdf)
ENDIF ()

IF (NOT DEFINED PDFCSP_LINK_PATH)
    SET(PDFCSP_LINK_PATH csp_c_bridge)
ENDIF ()

IF (NOT DEFINED PDFCSP_PDF_LINK_PATH)
    SET(PDFCSP_PDF_LINK_PATH csppdf)
ENDIF ()

IF (NOT DEFINED PDFCSP_INCLUDE_DIR)
    SET(PDFCSP_INCLUDE_DIR /usr/include/pdfcsp/)
ENDIF ()

IF (NOT DEFINED PDFCSP_PDF_INCLUDE_DIR)
    SET(PDFCSP_PDF_INCLUDE_DIR /usr/include/pdfcsp/)
ENDIF ()


#translations
IF (NOT DEFINED SIBLING_DEPLOY) #dont update translation for every sibling
    add_custom_target(update_translations ALL
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            COMMAND /usr/bin/lupdate-qt6 -recursive -locations relative . -ts resources/translations/ru_RU.ts
            COMMAND /usr/bin/lrelease-qt6 resources/translations/ru_RU.ts
            COMMENT "Update and compile translations"
            BYPRODUCTS ${CMAKE_SOURCE_DIR}/resources/translations/ru_RU.qm
    )
ENDIF ()

MESSAGE("Qt6 version minor ${Qt6_VERSION_MINOR}")


# QQmlApplicationEngine::loadFromModule since 6.5
IF (${Qt6_VERSION_MINOR} VERSION_LESS 5)
    qt_add_executable(surguch
            main.cpp
            cpp_views/pdf_page_render.hpp cpp_views/pdf_page_render.cpp
            cpp_views/preview_render.hpp cpp_views/preview_render.cpp
            cpp_views/rubber_preview_render.hpp cpp_views/rubber_preview_render.cpp
            ${CMAKE_SOURCE_DIR}/resources/resources.qrc
            ${CMAKE_SOURCE_DIR}/src/views/qml_sources.qrc
    )
    add_definitions(-DQT_LOAD_FROM_MODULE=0)
ELSE ()

    qt_policy(SET QTP0001 NEW)
    qt_policy(SET QTP0004 NEW)

    add_definitions(-DQT_LOAD_FROM_MODULE=1)
    qt_add_executable(surguch
            main.cpp
            ${CMAKE_SOURCE_DIR}/resources/resources.qrc
    )

    qt_add_qml_module(surguch
            URI gui_pdf_csp
            VERSION 1.0
            QML_FILES views/Main.qml
            QML_FILES views/header_bar_components/TopBarButton.qml
            QML_FILES views/header_bar_components/SubBarButton.qml
            QML_FILES views/Header.qml
            QML_FILES views/header_bar_components/HeaderToolSeparator.qml
            QML_FILES views/HeaderSubBar.qml
            QML_FILES views/PdfListView.qml
            SOURCES cpp_views/pdf_page_render.hpp cpp_views/pdf_page_render.cpp
            QML_FILES views/LeftSideBar.qml
            QML_FILES views/PreviewListView.qml
            QML_FILES views/info_panel_components/SignaturesList.qml
            QML_FILES views/RightSideBar.qml
            QML_FILES views/info_panel_components/TextPairBool.qml
            QML_FILES views/info_panel_components/RightSBHorizontalDelimiter.qml
            QML_FILES views/info_panel_components/TextPair.qml
            QML_FILES views/info_panel_components/RSideBarStatusMedal.qml
            QML_FILES views/info_panel_components/RightSideBarCertChain.qml
            QML_FILES views/info_panel_components/TextPairStatus.qml
            QML_FILES views/CertInfo.qml
            QML_FILES views/EditProfile.qml
            QML_FILES views/info_panel_components/RSBSigInfo.qml
            QML_FILES views/info_panel_components/RSBCloseButton.qml
            QML_FILES views/info_panel_components/RSBSwitch.qml
            QML_FILES views/info_panel_components/RSBComboSelect.qml
            QML_FILES views/info_panel_components/RSBTextArea.qml
            QML_FILES views/UnsavedChangesDialog.qml
            QML_FILES views/StampEditor.qml
            SOURCES cpp_views/preview_render.hpp cpp_views/preview_render.cpp
            QML_FILES views/SearchDialog.qml
            SOURCES cpp_views/rubber_preview_render.hpp cpp_views/rubber_preview_render.cpp
            QML_FILES views/RubberStampEditor.qml
            QML_FILES views/RubberStampDialog.qml
            QML_FILES views/RubberStampListEntry.qml
            QML_FILES views/InfoDialog.qml
            QML_FILES views/stamp_editor_components/SettingSwitch.qml
            QML_FILES views/stamp_editor_components/ColorSlider.qml
            QML_FILES views/stamp_editor_components/RGBColorPicker.qml
            QML_FILES views/stamp_editor_components/BorderSettings.qml
            QML_FILES views/stamp_editor_components/RubberPreview.qml
            QML_FILES views/stamp_editor_components/SettingSlider.qml
            QML_FILES views/stamp_editor_components/RubberPreviewLeftPanel.qml
            QML_FILES views/stamp_editor_components/TopLabelWithCloseButton.qml
            QML_FILES views/stamp_editor_components/StampNameInput.qml
            QML_FILES views/stamp_editor_components/TransparencySwitch.qml
            QML_FILES views/stamp_editor_components/RubberPreviewRightPanel.qml
            QML_FILES views/stamp_editor_components/StampPreview.qml
            QML_FILES views/SigCreatorWrapper.qml
            QML_FILES views/DisappearingHint.qml
    )

    qt6_import_qml_plugins(surguch)
ENDIF ()


IF (NOT DEFINED SIBLING_DEPLOY)
    add_dependencies(surguch update_translations)
ENDIF ()


if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    add_compile_definitions(WITH_QTEST=1)
endif ()

add_subdirectory(core)

add_subdirectory(models)

target_link_libraries(surguch PRIVATE
        appcore
)

target_link_libraries(surguch PUBLIC gui_core)

target_link_libraries(surguch PRIVATE
        models
)

target_link_libraries(surguch PRIVATE
        csp_c_bridge_qt
)

target_link_libraries(surguch
        PRIVATE Qt6::Quick
)

target_link_libraries(surguch
        PRIVATE Qt6::Widgets
)

target_link_libraries(surguch
        PRIVATE Qt6::Svg
)

target_link_libraries(surguch
        PRIVATE Qt6::Concurrent)

target_link_libraries(surguch PRIVATE Qt6::Core)

target_include_directories(surguch PUBLIC ${CMAKE_SOURCE_DIR}/src/)

# harcoded mupdf linkage
target_link_libraries(surguch
        PRIVATE ${MUPDF_LINK_PATH}
)
target_include_directories(surguch PRIVATE ${MUPDF_INCLUDE_DIR})

# alt csp lib linkage
# harcoded mupdf linkage
target_link_libraries(surguch
        PRIVATE ${PDFCSP_LINK_PATH}
)

target_link_libraries(surguch
        PRIVATE ${PDFCSP_PDF_LINK_PATH}
)

target_link_libraries(surguch PRIVATE Qt6::PrintSupport)

include(GNUInstallDirs)


if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    add_compile_definitions(WITH_QTEST=1)
    message("Building tests")
    add_subdirectory(test)
endif ()

if (NOT DEFINED SHARE_INSTALL_PREFIX)
    SET(SHARE_INSTALL_PREFIX /usr/share)
endif ()


install(TARGETS surguch RUNTIME)
install(FILES surguch.desktop DESTINATION ${SHARE_INSTALL_PREFIX}/applications PERMISSIONS WORLD_READ WORLD_EXECUTE)

IF (NOT DEFINED APP_ICON_DIR)
    SET(APP_ICON_DIR /usr/share/icons/hicolor/scalable/apps/)
ENDIF ()
IF (NOT DEFINED APP_ICON_PNG_DIR)
    SET(APP_ICON_PNG_DIR /usr/share/pixmaps/)
ENDIF ()

install(FILES ${CMAKE_SOURCE_DIR}/resources/app_icons/SealWax-1_32.svg DESTINATION ${APP_ICON_DIR} PERMISSIONS WORLD_READ)
install(FILES ${CMAKE_SOURCE_DIR}/resources/app_icons/SealWax-1_32.png DESTINATION ${APP_ICON_PNG_DIR} PERMISSIONS WORLD_READ)
