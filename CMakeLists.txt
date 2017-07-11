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

add_library(kengine INTERFACE)
target_link_libraries(kengine INTERFACE mediator pluginManager)
target_include_directories(kengine INTERFACE .)

if(PUTILS_BUILD_PSE)
    add_subdirectory(common/systems/sfml)
endif()

