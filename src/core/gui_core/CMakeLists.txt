add_library(gui_core OBJECT
        history_manager.cpp
        history_manager.hpp
        rubber_structs.hpp
        gui_utils.cpp
        gui_utils.hpp
        custom_event_filter.hpp
)

target_include_directories(gui_core PUBLIC csp_c_bridge)
target_include_directories(gui_core PRIVATE .)


target_include_directories(gui_core PUBLIC
        ${PDFCSP_PDF_INCLUDE_DIR}
)

target_link_libraries(gui_core
        PRIVATE ${PDFCSP_LINK_PATH}
)

target_link_libraries(gui_core
        PRIVATE ${PDFCSP_PDF_LINK_PATH}
)

target_link_libraries(gui_core
        PRIVATE ${PDFCSP_PDF_LINK_PATH}
)

target_link_libraries(gui_core PRIVATE Qt6::Core)
target_link_libraries(gui_core PRIVATE Qt6::Concurrent)
target_link_libraries(gui_core PRIVATE Qt6::Gui)
