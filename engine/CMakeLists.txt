cmake_minimum_required(VERSION 3.0)
project(kengine)

include_directories(include)

file(GLOB src_files
        src/*.cpp
        include/*.hpp
        )

add_library(kengine ${src_files})