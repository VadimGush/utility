cmake_minimum_required(VERSION 3.25)
project(utilities)

set(CMAKE_CXX_STANDARD 20)

include_directories(/usr/local/include)

add_executable(test main.cpp)
