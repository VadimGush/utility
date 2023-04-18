cmake_minimum_required(VERSION 3.25)
project(utilities)

set(CMAKE_CXX_STANDARD 20)

# GOOGLE TESTS
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
FetchContent_MakeAvailable(googletest)

enable_testing()

include_directories(/usr/local/include)

add_executable(utils_test test/collections_result_test.cpp test/unix_read_file_test.cpp test/qtree_test.cpp test/math_test.cpp)
target_link_libraries(utils_test GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(utils_test)
