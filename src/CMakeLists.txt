find_package(Qt6 6.5 REQUIRED COMPONENTS Quick REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Svg REQUIRED)

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
)


target_link_libraries(appgui_pdf_csp
    PRIVATE Qt6::Quick
)

target_link_libraries(appgui_pdf_csp
     PRIVATE Qt6::Svg
)


include(GNUInstallDirs)

