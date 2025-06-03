find_package(Qt6 REQUIRED COMPONENTS Test REQUIRED)

set(CMAKE_AUTOMOC ON)

enable_testing(true)

include_directories(${CMAKE_SOURCE_DIR}/src/)
include_directories(${MUPDF_INCLUDE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/src/core/csp_c_bridge)
include_directories(${PDFCSP_INCLUDE_DIR})
include_directories(${PDFCSP_INCLUDE_DIR}csp/checks)
include_directories(${PDFCSP_INCLUDE_DIR}/csp)

#-------------------------------
#common test

qt_add_executable(test1 test_common.cpp
        t_signature.hpp t_signature.cpp)
add_compile_definitions(test1 PUBLIC -DTEST_FILES_DIR="${CMAKE_SOURCE_DIR}/test_files/")

#-------------------------------
# search

qt_add_executable(test_search
        test_search.cpp
        tsearch.hpp tsearch.cpp)

qt_add_executable(test_search2
        tsearch_multhreaded.cpp)
add_compile_definitions(test_search2 PUBLIC -DTEST_FILES_DIR="${CMAKE_SOURCE_DIR}/test_files/")

#-------------------------------
# preview

qt_add_executable(test_preview test_preview.cpp
        t_stamp.hpp t_stamp.cpp
        ${CMAKE_SOURCE_DIR}/src/cpp_views/preview_render.hpp
        ${CMAKE_SOURCE_DIR}/src/cpp_views/preview_render.cpp
)

#-------------------------------
# rubber stamps

add_executable(test_rubber test_rubber.cpp
        t_rubber.cpp t_rubber.hpp
        ${CMAKE_SOURCE_DIR}/src/cpp_views/rubber_preview_render.hpp
        ${CMAKE_SOURCE_DIR}/src/cpp_views/rubber_preview_render.cpp
)

#-------------------------------
#link test1

target_link_libraries(test1 PRIVATE Qt::Test Qt6::Quick)
target_link_libraries(test1 PRIVATE appcore)
target_link_libraries(test1 PRIVATE gui_core)
target_link_libraries(test1 PRIVATE models)
target_link_libraries(test1 PRIVATE csp_c_bridge_qt)
target_link_libraries(test1 PRIVATE ${PDFCSP_LINK_PATH})
# harcoded mupdf linkage
target_link_libraries(test1 PRIVATE ${MUPDF_LINK_PATH})
target_compile_definitions(test1 PUBLIC -DTEST_FILES_DIR="${CMAKE_SOURCE_DIR}/test_files/")

#-------------------------------
# link test_preview

target_link_libraries(test_preview PUBLIC Qt::Test Qt6::Quick Qt6::Concurrent)
target_link_libraries(test_preview PUBLIC appcore)
target_link_libraries(test_preview PUBLIC gui_core)
target_link_libraries(test_preview PUBLIC models)
target_link_libraries(test_preview PUBLIC csp_c_bridge_qt)
target_link_libraries(test_preview PUBLIC ${PDFCSP_LINK_PATH})
target_link_libraries(test_preview PRIVATE ${MUPDF_LINK_PATH}) # harcoded mupdf linkage
target_compile_definitions(test_preview PUBLIC -DTEST_FILES_DIR="${CMAKE_SOURCE_DIR}/test_files/")

#-------------------------------
# link test_rubber

target_link_libraries(test_rubber PUBLIC Qt::Test Qt6::Quick Qt6::Concurrent)
target_link_libraries(test_rubber PUBLIC appcore)
target_link_libraries(test_rubber PUBLIC gui_core)
target_link_libraries(test_rubber PUBLIC models)
target_link_libraries(test_rubber PUBLIC csp_c_bridge_qt)
target_link_libraries(test_rubber PUBLIC ${PDFCSP_LINK_PATH})
target_link_libraries(test_rubber PRIVATE ${MUPDF_LINK_PATH}) # harcoded mupdf linkage
target_compile_definitions(test_rubber PUBLIC -DTEST_FILES_DIR="${CMAKE_SOURCE_DIR}/test_files/")

#-------------------------------
# link test_search
target_link_libraries(test_search PUBLIC Qt::Test)
target_link_libraries(test_search PUBLIC appcore csp_c_bridge_qt)
target_link_libraries(test_search
        PUBLIC ${MUPDF_LINK_PATH} PRIVATE ${PDFCSP_LINK_PATH}
)
target_link_libraries(test_search2 PUBLIC appcore csp_c_bridge_qt)
target_link_libraries(test_search2
        PUBLIC ${MUPDF_LINK_PATH} PRIVATE ${PDFCSP_LINK_PATH}
)


#-------------------------------
