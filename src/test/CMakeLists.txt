find_package(Qt6 REQUIRED COMPONENTS Test REQUIRED)

set(CMAKE_AUTOMOC ON)

enable_testing(true)

qt_add_executable(test1 test_common.cpp
    t_signature.hpp t_signature.cpp)

add_test(NAME test1 COMMAND mytest)

target_link_libraries(test1 PRIVATE Qt::Test)
target_link_libraries(test1  PRIVATE appcore)
target_include_directories(test1 PRIVATE ${QtTest_INCLUDE_DIRS})
message("QTest dir ${QtTest_INCLUDE_DIRS}")


# harcoded mupdf linkage
target_link_libraries(test1
     PRIVATE /home/oleg/dev/mupdf/build/shared-release/libmupdf.so.25.0
     #PRIVATE /home/oleg/dev/mupdf/build/shared-debug/libmupdf.so.25.0
)
