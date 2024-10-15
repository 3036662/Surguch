find_package(Qt6 6.5 REQUIRED COMPONENTS Quick REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Svg REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Widgets REQUIRED)
#find_package(Qt6 REQUIRED COMPONENTS Pdf REQUIRED)




set(CMAKE_AUTORCC ON)

qt_add_executable(appgui_pdf_csp
    main.cpp
    ${CMAKE_SOURCE_DIR}/resources/resources.qrc
)

#target_compile_options(appgui_pdf_csp PRIVATE -fsanitize=address)
#target_link_libraries(appgui_pdf_csp PRIVATE -fsanitize=address)
#target_compile_options(appgui_pdf_csp PRIVATE -fsanitize=memory)
#target_link_libraries(appgui_pdf_csp PRIVATE -fsanitize=memory)


SET (MUPDF_INCLUDE_DIR /home/oleg/dev/mupdf/include/)
SET (MUPDF_LINK_PATH /home/oleg/dev/mupdf/build/shared-release/libmupdf.so.25.0)
#SET (MUPDF_LINK_PATH /home/oleg/dev/mupdf/build/shared-debug/libmupdf.so.25.0)

SET (PDFCSP_LINK_PATH /home/oleg/dev/eSign/csp_pdf/build/c_bridge/libcsp_c_bridge.so)
SET (PDFCSP_INCLUDE_DIR /home/oleg/dev/eSign/csp_pdf/)


qt_add_qml_module(appgui_pdf_csp
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
    SOURCES
    QML_FILES views/RightSBHorizontalDelimiter.qml
    QML_FILES views/TextPair.qml
    QML_FILES views/RSideBarStatusMedal.qml
    QML_FILES views/RightSideBarCertChain.qml
    QML_FILES views/TextPairStatus.qml
    QML_FILES views/CertInfo.qml
)

add_subdirectory(core)

add_subdirectory(models)

target_link_libraries(appgui_pdf_csp PRIVATE
    appcore
)

target_link_libraries(appgui_pdf_csp PRIVATE
    models
)

target_link_libraries(appgui_pdf_csp PRIVATE
    csp_c_bridge
)


target_link_libraries(appgui_pdf_csp
    PRIVATE Qt6::Quick
)

target_link_libraries(appgui_pdf_csp
    PRIVATE Qt6::Widgets
)

target_link_libraries(appgui_pdf_csp
     PRIVATE Qt6::Svg
)

target_link_libraries(appgui_pdf_csp PRIVATE Qt6::Core)

target_include_directories(appgui_pdf_csp PUBLIC ${CMAKE_SOURCE_DIR}/src/)

#target_link_libraries(appgui_pdf_csp PRIVATE Qt6::Pdf)


# harcoded mupdf linkage
target_link_libraries(appgui_pdf_csp
     PRIVATE ${MUPDF_LINK_PATH}
)
target_include_directories(appgui_pdf_csp PRIVATE ${MUPDF_INCLUDE_DIR})

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    message("Building tests")
    add_subdirectory(test)
endif()

# alt csp lib linkage
# harcoded mupdf linkage
target_link_libraries(appgui_pdf_csp
     PRIVATE ${PDFCSP_LINK_PATH}
)


include(GNUInstallDirs)

