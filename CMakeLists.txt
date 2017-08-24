cmake_minimum_required(VERSION 3.0)
project(kengine)
set(CMAKE_CXX_STANDARD 17)

if(KENGINE_SFML)
    set(PUTILS_BUILD_PSE TRUE)
endif()
if(KENGINE_OGRE)
    set(PUTILS_BUILD_POGRE TRUE)
endif()

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

if(KENGINE_SFML)
    add_subdirectory(common/systems/sfml)
endif()

if(KENGINE_OGRE)
    add_subdirectory(common/systems/ogre)
endif()
