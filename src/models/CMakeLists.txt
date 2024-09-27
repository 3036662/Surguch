add_library(models OBJECT
  pdf_page_model.cpp pdf_page_model.hpp
  signatures_list_model.hpp signatures_list_model.cpp
)

target_include_directories(models PRIVATE ${MUPDF_INCLUDE_DIR})
target_include_directories(models PRIVATE ${CMAKE_SOURCE_DIR}/src)

target_link_libraries(models PRIVATE Qt6::Core)
