cmake_minimum_required(VERSION 3.0)
project(kengine)
set(CMAKE_CXX_STANDARD 17)

if (KENGINE_SFML)
    set(TGUI TRUE)
    set(PUTILS_BUILD_PSE TRUE)
endif ()

if (KENGINE_OGRE)
    set(PUTILS_BUILD_POGRE TRUE)
endif ()

if (KENGINE_TEST)
    set(PUTILS_TEST TRUE)
endif ()

if(KENGINE_LUA)
    set(PUTILS_BUILD_LUA TRUE)
	set(ADDITIONAL_LIBS ${ADDITIONAL_LIBS} lua)
endif()

if(KENGINE_PYTHON)
    set(PUTILS_BUILD_PYTHON TRUE)
	set(ADDITIONAL_LIBS ${ADDITIONAL_LIBS} puthon)
endif()

set(PUTILS_BUILD_MEDIATOR TRUE)
add_subdirectory(putils)

file(GLOB src_files
        *.cpp
        *.hpp
        )

if (UNIX)
    set(type SHARED)
elseif (WIN32)
    set(type STATIC)
endif ()

add_library(kengine ${type} Component.cpp)
target_link_libraries(kengine PUBLIC mediator pluginManager ${ADDITIONAL_LIBS})
target_include_directories(kengine PUBLIC . common)

if (KENGINE_SFML)
    add_subdirectory(common/systems/sfml)
endif ()

if (KENGINE_OGRE)
    add_subdirectory(common/systems/ogre)
endif ()

if (KENGINE_B2D)
    add_subdirectory(common/systems/box2d)
endif ()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} PARENT_SCOPE)
