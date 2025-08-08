add_library(models OBJECT
        pdf_doc_model.cpp
        pdf_doc_model.hpp
        signatures_list_model.hpp
        signatures_list_model.cpp
        profiles_model.hpp
        profiles_model.cpp
        rubber_stamp_model.cpp
        rubber_stamp_model.hpp
        file_tree_model.hpp
        file_tree_model.cpp
        tree_item.cpp
        tree_item.hpp
)

target_include_directories(models PRIVATE ${MUPDF_INCLUDE_DIR})
target_include_directories(models PRIVATE ${CMAKE_SOURCE_DIR}/src)

target_include_directories(models PRIVATE ${CMAKE_SOURCE_DIR}/src/core/csp_c_bridge)
target_include_directories(models PRIVATE ${CMAKE_SOURCE_DIR}/src/core)

target_include_directories(models PUBLIC
        ${PDFCSP_INCLUDE_DIR}
)

target_include_directories(models PUBLIC
        ${PDFCSP_PDF_INCLUDE_DIR}
)

target_include_directories(models PUBLIC
        ${PDFCSP_MRPA_INCLUDE_DIR}
)

target_include_directories(models PUBLIC
        ${PDFCSP_MRPA_INCLUDE_DIR}/tree
)

target_include_directories(models PUBLIC
        ${PDFCSP_MRPA_INCLUDE_DIR}/zip
)

target_include_directories(models PUBLIC
        ${PDFCSP_INCLUDE_DIR}csp/checks
)
target_include_directories(models PUBLIC
        ${PDFCSP_INCLUDE_DIR}csp
)

target_include_directories(models PUBLIC
        ${PDFCSP_INCLUDE_DIR}/c_bridge
)

target_link_libraries(models PRIVATE gui_core)

target_link_libraries(models PRIVATE Qt6::Concurrent)

target_link_libraries(models PRIVATE Qt6::Core)

target_link_libraries(models PRIVATE Qt6::Gui)

