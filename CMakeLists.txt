cmake_minimum_required(VERSION 3.0)
project(kengine)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -Werror -std=c++14")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin/)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib/)

include_directories(include/core)
include_directories(include/test)

file(GLOB core_src_files
        src/core/*.cpp
        include/core/*.hpp
        )

file(GLOB test_src_files
        src/test/*.cpp
        include/test/*.hpp
        )

add_executable(ktest ${test_src_files})
add_library(kengine SHARED ${core_src_files})
target_link_libraries(ktest ${PROJECT_SOURCE_DIR}/lib/libkengine.so)