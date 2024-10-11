add_library (csp_c_bridge OBJECT
    csp_response.cpp
    bridge_utils.cpp
)

target_include_directories(csp_c_bridge PUBLIC
    ${PDFCSP_INCLUDE_DIR}
)

target_include_directories(csp_c_bridge PUBLIC
    ${PDFCSP_INCLUDE_DIR}csp/checks
)

target_include_directories(csp_c_bridge PUBLIC
    ${PDFCSP_INCLUDE_DIR}csp/
)

target_include_directories(csp_c_bridge PUBLIC
    ${PDFCSP_INCLUDE_DIR}c_bridge
)

target_include_directories(csp_c_bridge PUBLIC
    ${CMAKE_SOURCE_DIR}/src/core/
)

target_include_directories(csp_c_bridge PRIVATE ${MUPDF_INCLUDE_DIR})

target_link_libraries(csp_c_bridge PRIVATE Qt6::Core)
