find_package(Qt6 REQUIRED COMPONENTS Test REQUIRED)

set(CMAKE_AUTOMOC ON)

enable_testing(true)

include_directories(${CMAKE_SOURCE_DIR}/src/)
include_directories(${MUPDF_INCLUDE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/src/core/csp_c_bridge)
include_directories(${PDFCSP_INCLUDE_DIR})
include_directories(${PDFCSP_INCLUDE_DIR}csp/checks)
include_directories(${PDFCSP_INCLUDE_DIR}/csp)

file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/testfiles/")
set(TEST_DIR "${CMAKE_BINARY_DIR}/testfiles/")
set(TEST_FILES_DIR "${CMAKE_SOURCE_DIR}/test_files/")
# clone repo with test data to test_files
set(SENSITIVE_DIR "${CMAKE_SOURCE_DIR}/test_files/csp_test_data/")
message("Test files will be created here:${TEST_DIR}/test_files")
add_definitions(-DTEST_DIR="${TEST_DIR}")
add_definitions(-DTEST_FILES_DIR="${TEST_FILES_DIR}")
add_definitions(-DSENSITIVE_DIR="${SENSITIVE_DIR}")


#-------------------------------
#common test

qt_add_executable(test1 test_common.cpp
        t_signature.hpp t_signature.cpp)


#-------------------------------
# search

qt_add_executable(test_search
        test_search.cpp
        tsearch.hpp tsearch.cpp)

qt_add_executable(test_search2
        tsearch_multhreaded.cpp)


#-------------------------------
# preview

qt_add_executable(test_preview test_preview.cpp
        t_stamp.hpp t_stamp.cpp
        ${CMAKE_SOURCE_DIR}/src/cpp_views/preview_render.hpp
        ${CMAKE_SOURCE_DIR}/src/cpp_views/preview_render.cpp
)

#-------------------------------
# rubber

qt_add_executable(test_rubber test_rubber.cpp
        t_rubber.hpp t_rubber.cpp
        ${CMAKE_SOURCE_DIR}/src/cpp_views/rubber_preview_render.hpp
        ${CMAKE_SOURCE_DIR}/src/cpp_views/rubber_preview_render.cpp
)

#-------------------------------
# gui utils

qt_add_executable(test_gui_utils test_guiutils.cpp
        t_guiutils.hpp t_guiutils.cpp
)

#-------------------------------
# golink
qt_add_executable(test_golink test_golink.cpp
    tgolink.hpp tgolink.cpp
)

#-------------------------------
# file tree
qt_add_executable(test_tree test_tree.cpp
    t_tree.hpp t_tree.cpp
    ${CMAKE_SOURCE_DIR}/src/models/tree_item.hpp
    ${CMAKE_SOURCE_DIR}/src/models/tree_item.cpp
    ${CMAKE_SOURCE_DIR}/src/models/file_tree_model.hpp
    ${CMAKE_SOURCE_DIR}/src/models/file_tree_model.cpp
    mocked_tree.hpp
)
target_compile_definitions(test_tree PRIVATE MOC_TREE_FOR_TESTS=1)

#-------------------------------
#link test1

target_link_libraries(test1 PRIVATE Qt::Test Qt6::Quick)
target_link_libraries(test1 PRIVATE appcore)
target_link_libraries(test1 PUBLIC gui_core)
target_link_libraries(test1 PRIVATE models)
target_link_libraries(test1 PRIVATE csp_c_bridge_qt)
target_link_libraries(test1 PRIVATE ${PDFCSP_LINK_PATH})
target_link_libraries(test1 PRIVATE ${PDFCSP_MRPA_LINK_PATH})
# harcoded mupdf linkage
target_link_libraries(test1 PRIVATE ${MUPDF_LINK_PATH})


#-------------------------------
# ling test_preview

target_link_libraries(test_preview PUBLIC Qt::Test Qt6::Quick Qt6::Concurrent)
target_link_libraries(test_preview PUBLIC appcore)
target_link_libraries(test_preview PUBLIC gui_core)
target_link_libraries(test_preview PUBLIC models)
target_link_libraries(test_preview PUBLIC csp_c_bridge_qt)
target_link_libraries(test_preview PUBLIC ${PDFCSP_LINK_PATH})
target_link_libraries(test_preview PRIVATE ${MUPDF_LINK_PATH}) # harcoded mupdf linkage
target_link_libraries(test_preview PRIVATE ${PDFCSP_MRPA_LINK_PATH})

#-------------------------------
# ling test_rubber

target_link_libraries(test_rubber PUBLIC Qt::Test Qt6::Quick Qt6::Concurrent)
target_link_libraries(test_rubber PUBLIC appcore)
target_link_libraries(test_rubber PUBLIC gui_core)
target_link_libraries(test_rubber PUBLIC models)
target_link_libraries(test_rubber PUBLIC csp_c_bridge_qt)
target_link_libraries(test_rubber PUBLIC ${PDFCSP_LINK_PATH})
target_link_libraries(test_rubber PRIVATE ${MUPDF_LINK_PATH}) # harcoded mupdf linkage
target_link_libraries(test_rubber PRIVATE ${PDFCSP_MRPA_LINK_PATH})

#-------------------------------
# ling test_gui_utils

target_link_libraries(test_gui_utils PUBLIC Qt::Test Qt6::Quick Qt6::Concurrent)
target_link_libraries(test_gui_utils PUBLIC appcore)
target_link_libraries(test_gui_utils PUBLIC gui_core)
target_link_libraries(test_gui_utils PUBLIC models)
target_link_libraries(test_gui_utils PUBLIC csp_c_bridge_qt)
target_link_libraries(test_gui_utils PUBLIC ${PDFCSP_LINK_PATH})
target_link_libraries(test_gui_utils PRIVATE ${MUPDF_LINK_PATH}) # harcoded mupdf linkage
target_link_libraries(test_gui_utils PRIVATE ${PDFCSP_MRPA_LINK_PATH})


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

#-------------------------------
# link test_golink
target_link_libraries(test_golink PUBLIC Qt::Test)
target_link_libraries(test_golink PUBLIC appcore csp_c_bridge_qt)
target_link_libraries(test_golink
    PUBLIC ${MUPDF_LINK_PATH}
    PRIVATE ${PDFCSP_LINK_PATH}
)

#-------------------------------

# link test_tree
target_link_libraries(test_tree PUBLIC Qt::Test Qt6::Quick Qt6::Concurrent)
target_link_libraries(test_tree PUBLIC appcore)
target_link_libraries(test_tree PUBLIC gui_core)
target_link_libraries(test_tree PUBLIC csp_c_bridge_qt)
target_link_libraries(test_tree PUBLIC ${PDFCSP_LINK_PATH})
target_link_libraries(test_tree PRIVATE ${MUPDF_LINK_PATH}) # harcoded mupdf linkage
target_link_libraries(test_tree PRIVATE ${PDFCSP_MRPA_LINK_PATH})

#-------------------------------
