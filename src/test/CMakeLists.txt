find_package(Qt6 REQUIRED COMPONENTS Test REQUIRED)

set(CMAKE_AUTOMOC ON)

enable_testing(true)

qt_add_executable(test1 test_common.cpp
    t_signature.hpp t_signature.cpp)

qt_add_executable(test_search
      test_search.cpp
      tsearch.hpp tsearch.cpp)

add_test(NAME test1 COMMAND test1)
add_test(NAME test_search COMMAND test_search)

include_directories(test1 PUBLIC ${CMAKE_SOURCE_DIR}/src/)
include_directories(test1 PUBLIC ${MUPDF_INCLUDE_DIR})

include_directories(test1 PUBLIC ${CMAKE_SOURCE_DIR}/src/core/csp_c_bridge)

include_directories(test1 PUBLIC
    ${PDFCSP_INCLUDE_DIR}
)

include_directories(test1 PUBLIC
    ${PDFCSP_INCLUDE_DIR}csp/checks
)

include_directories(test1 PUBLIC
    ${PDFCSP_INCLUDE_DIR}csp
)

target_link_libraries(test1 PRIVATE Qt::Test)
target_link_libraries(test1  PRIVATE appcore)
target_link_libraries(test1  PRIVATE models)
target_link_libraries(test1  PRIVATE csp_c_bridge_qt)

target_link_libraries(test1
     PRIVATE ${PDFCSP_LINK_PATH}
)

# harcoded mupdf linkage
target_link_libraries(test1 PRIVATE ${MUPDF_LINK_PATH})

add_compile_definitions(test1 PUBLIC -DTEST_FILES_DIR="${CMAKE_SOURCE_DIR}/test_files/")

target_link_libraries(test_search PUBLIC Qt::Test )
target_link_libraries(test_search PUBLIC appcore csp_c_bridge_qt)

target_link_libraries(test_search
     PUBLIC ${MUPDF_LINK_PATH}  PRIVATE ${PDFCSP_LINK_PATH}
)


