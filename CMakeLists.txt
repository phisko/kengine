cmake_minimum_required(VERSION 3.0)
project(kengine)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -std=c++14")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin/)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib/)

include_directories(test/include)
include_directories(engine/include)

add_subdirectory(engine)

file(GLOB test_src_files
        test/src/*.cpp
        test/include/*.hpp
        )

add_executable(ktest ${test_src_files})
target_link_libraries(ktest kengine)