add_library(appcore OBJECT
    mu_page_render.hpp mu_page_render.cpp
    signature_processor.hpp signature_processor.cpp
)

target_include_directories(appcore PRIVATE /home/oleg/dev/mupdf/include/)

target_link_libraries(appcore PRIVATE Qt6::Core)
