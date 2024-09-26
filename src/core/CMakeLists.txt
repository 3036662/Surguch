add_library(appcore OBJECT
    mu_page_render.hpp mu_page_render.cpp
    signature_processor.hpp signature_processor.cpp
    pdf_obj_keeper.hpp pdf_obj_keeper.cpp
    raw_signature.hpp raw_signature.cpp
    utils.hpp
    utils.cpp
)

target_include_directories(appcore PRIVATE ${MUPDF_INCLUDE_DIR})

target_link_libraries(appcore PRIVATE Qt6::Core)
