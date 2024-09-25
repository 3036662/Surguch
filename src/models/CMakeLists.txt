add_library(models OBJECT
  pdf_page_model.cpp pdf_page_model.hpp
)

target_include_directories(models PRIVATE ${MUPDF_INCLUDE_DIR})

target_link_libraries(models PRIVATE Qt6::Core)
