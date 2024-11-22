find_package(Qt6 6.4 REQUIRED COMPONENTS Quick REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Svg REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Widgets REQUIRED)


if (NOT DEFINED TRANSLATION_DIR)
    set(TRANSLATION_DIR ${CMAKE_BINARY_DIR}/src/)
endif()
message(TRANSLATION_DIR=${TRANSLATION_DIR})

set(CMAKE_AUTORCC ON)




add_compile_definitions(TRANSLATION_DIR="${TRANSLATION_DIR}")

#target_compile_options(sealwax PRIVATE -fsanitize=address)
#target_link_libraries(sealwax PRIVATE -fsanitize=address)
#target_compile_options(sealwax PRIVATE -fsanitize=memory)
#target_link_libraries(sealwax PRIVATE -fsanitize=memory)

IF(NOT DEFINED MUPDF_INCLUDE_DIR)
    SET (MUPDF_INCLUDE_DIR /usr/include/)
ENDIF()

IF(NOT DEFINED MUPDF_LINK_PATH)
    SET (MUPDF_LINK_PATH mupdf)
ENDIF()

IF(NOT DEFINED PDFCSP_LINK_PATH)
    SET (PDFCSP_LINK_PATH csp_c_bridge)
ENDIF()

IF (NOT DEFINED PDFCSP_PDF_LINK_PATH)
    SET (PDFCSP_PDF_LINK_PATH csppdf)
ENDIF()

IF (NOT DEFINED PDFCSP_INCLUDE_DIR)
    SET (PDFCSP_INCLUDE_DIR /usr/include/pdfcsp/)
ENDIF()

IF (NOT DEFINED PDFCSP_PDF_INCLUDE_DIR)
    SET (PDFCSP_PDF_INCLUDE_DIR /usr/include/pdfcsp/)
ENDIF()


#translations
add_custom_target(update_translations ALL
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMAND /usr/bin/lupdate-qt6 -recursive -locations relative . -ts resources/translations/ru_RU.ts
    COMMAND /usr/bin/lrelease-qt6 resources/translations/ru_RU.ts
    COMMENT "Update and compile translations"
    BYPRODUCTS ${CMAKE_SOURCE_DIR}/resources/translations/ru_RU.qm
)



# QQmlApplicationEngine::loadFromModule since 6.5
IF(${Qt6_VERSION_MINOR} VERSION_LESS 5 )
    qt_add_executable(sealwax
        main.cpp
        cpp_views/pdf_page_render.hpp cpp_views/pdf_page_render.cpp
        ${CMAKE_SOURCE_DIR}/resources/resources.qrc
        ${CMAKE_SOURCE_DIR}/src/views/qml_sources.qrc
    )
    add_definitions(-DQT_LOAD_FROM_MODULE=0)
ELSE()
    add_definitions(-DQT_LOAD_FROM_MODULE=1)
    qt_add_executable(sealwax
        main.cpp
        ${CMAKE_SOURCE_DIR}/resources/resources.qrc
    )

    qt_add_qml_module(sealwax
        URI gui_pdf_csp
        VERSION 1.0
        QML_FILES views/Main.qml
        QML_FILES views/TopBarButton.qml
        QML_FILES views/Header.qml
        QML_FILES views/HeaderToolSeparator.qml
        QML_FILES views/HeaderSubBar.qml
        QML_FILES views/PdfListView.qml
        SOURCES cpp_views/pdf_page_render.hpp cpp_views/pdf_page_render.cpp
        QML_FILES views/LeftSideBar.qml
        QML_FILES views/PreviewListView.qml
        QML_FILES views/SignaturesList.qml
        QML_FILES views/RightSideBar.qml
        QML_FILES views/TextPairBool.qml
        QML_FILES views/RightSBHorizontalDelimiter.qml
        QML_FILES views/TextPair.qml
        QML_FILES views/RSideBarStatusMedal.qml
        QML_FILES views/RightSideBarCertChain.qml
        QML_FILES views/TextPairStatus.qml
        QML_FILES views/CertInfo.qml
        QML_FILES views/EditProfile.qml
        QML_FILES views/RSBSigInfo.qml
        QML_FILES views/RSBCloseButton.qml
        QML_FILES views/RSBSwitch.qml
        QML_FILES views/RSBComboSelect.qml
        QML_FILES views/RSBTextArea.qml
    )
ENDIF()

add_dependencies(sealwax update_translations)


if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    add_compile_definitions(WITH_QTEST=1)
endif()

add_subdirectory(core)

add_subdirectory(models)

target_link_libraries(sealwax PRIVATE
    appcore
)

target_link_libraries(sealwax PRIVATE
    models
)

target_link_libraries(sealwax PRIVATE
    csp_c_bridge_qt
)


target_link_libraries(sealwax
    PRIVATE Qt6::Quick
)

target_link_libraries(sealwax
    PRIVATE Qt6::Widgets
)

target_link_libraries(sealwax
     PRIVATE Qt6::Svg
)

target_link_libraries(sealwax PRIVATE Qt6::Core)

target_include_directories(sealwax PUBLIC ${CMAKE_SOURCE_DIR}/src/)

# harcoded mupdf linkage
target_link_libraries(sealwax
     PRIVATE ${MUPDF_LINK_PATH}
)
target_include_directories(sealwax PRIVATE ${MUPDF_INCLUDE_DIR})

# alt csp lib linkage
# harcoded mupdf linkage
target_link_libraries(sealwax
     PRIVATE ${PDFCSP_LINK_PATH}
)

target_link_libraries(sealwax
    PRIVATE ${PDFCSP_PDF_LINK_PATH}
)

include(GNUInstallDirs)


if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    add_compile_definitions(WITH_QTEST=1)
    message("Building tests")
    add_subdirectory(test)
endif()

if (NOT DEFINED SHARE_INSTALL_PREFIX)
	SET (SHARE_INSTALL_PREFIX /usr/share)
endif()	


install(TARGETS  sealwax RUNTIME)
install(FILES sealwax.desktop DESTINATION ${SHARE_INSTALL_PREFIX}/applications PERMISSIONS WORLD_READ WORLD_EXECUTE)
