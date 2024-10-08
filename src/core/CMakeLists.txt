add_library(appcore OBJECT
    mu_page_render.hpp mu_page_render.cpp
    signature_processor.hpp signature_processor.cpp
    pdf_obj_keeper.hpp pdf_obj_keeper.cpp
    raw_signature.hpp raw_signature.cpp
    utils.hpp
    utils.cpp
    signatures_validator.hpp signatures_validator.cpp
)

add_subdirectory(csp_c_bridge)

target_include_directories(appcore PUBLIC
    ${PDFCSP_INCLUDE_DIR}/c_bridge
)

target_include_directories(appcore PRIVATE ${MUPDF_INCLUDE_DIR})
target_include_directories(appcore PUBLIC csp_c_bridge)
target_include_directories(appcore PRIVATE .)



target_link_libraries(appcore PUBLIC csp_c_bridge)
target_link_libraries(appcore PRIVATE Qt6::Core)
