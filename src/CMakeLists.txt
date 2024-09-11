find_package(Qt6 6.5 REQUIRED COMPONENTS Quick REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Svg REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Widgets REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Pdf REQUIRED)

set(CMAKE_AUTORCC ON)

qt_add_executable(appgui_pdf_csp
    main.cpp
    ${CMAKE_SOURCE_DIR}/resources/resources.qrc
)

qt_add_qml_module(appgui_pdf_csp
    URI gui_pdf_csp
    VERSION 1.0
    QML_FILES
        views/Main.qml
        QML_FILES views/TopBarButton.qml
        QML_FILES views/Header.qml
        QML_FILES views/HeaderToolSeparator.qml
        QML_FILES views/HeaderSubBar.qml
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

target_link_libraries(appgui_pdf_csp PRIVATE Qt6::Pdf)


include(GNUInstallDirs)

