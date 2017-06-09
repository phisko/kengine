cmake_minimum_required(VERSION 3.0)
project(kengine)

add_subdirectory(putils)

file(GLOB src_files
        *.cpp
        *.hpp
        )

if(UNIX)
    set(type SHARED)
elseif(WIN32)
    set(type STATIC)
endif()

add_library(kengine ${type} ${src_files})
target_link_libraries(kengine mediator)
target_include_directories(kengine PUBLIC .)
