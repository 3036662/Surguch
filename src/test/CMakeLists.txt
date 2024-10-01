find_package(Qt6 REQUIRED COMPONENTS Test REQUIRED)

set(CMAKE_AUTOMOC ON)

enable_testing(true)

qt_add_executable(test1 test_common.cpp
    t_signature.hpp t_signature.cpp)

add_test(NAME test1 COMMAND mytest)

target_include_directories(test1 PUBLIC ${CMAKE_SOURCE_DIR}/src/)
target_include_directories(test1 PRIVATE ${MUPDF_INCLUDE_DIR})

target_link_libraries(test1 PRIVATE Qt::Test)
target_link_libraries(test1  PRIVATE appcore)
target_link_libraries(test1  PRIVATE models)
target_link_libraries(test1  PRIVATE csp_c_bridge)

target_link_libraries(test1
     PRIVATE ${PDFCSP_LINK_PATH}
)

# harcoded mupdf linkage
target_link_libraries(test1 PRIVATE ${MUPDF_LINK_PATH})

target_compile_definitions(test1 PUBLIC -DTEST_FILES_DIR="${CMAKE_SOURCE_DIR}/test_files/")
