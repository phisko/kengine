cmake_minimum_required(VERSION 3.0)
project(kengine)

include_directories(include)

file(GLOB src_files
        src/*.cpp
        include/*.hpp
        )

add_library(kengine SHARED ${src_files})
target_link_libraries(kengine sfml-graphics sfml-window sfml-system)