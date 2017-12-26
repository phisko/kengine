cmake_minimum_required(VERSION 3.0)
project(kengine)
set(CMAKE_CXX_STANDARD 17)

if (KENGINE_SFML)
    set(PUTILS_BUILD_PSE TRUE)
endif ()
if (KENGINE_OGRE)
    set(PUTILS_BUILD_POGRE TRUE)
endif ()

if (KENGINE_TEST)
    set(PUTILS_TEST TRUE)
endif ()

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

add_library(kengine INTERFACE)
target_link_libraries(kengine INTERFACE mediator pluginManager)
target_include_directories(kengine INTERFACE .)

if (KENGINE_SFML)
    add_subdirectory(common/systems/sfml)
endif ()

if (KENGINE_OGRE)
    add_subdirectory(common/systems/ogre)
endif ()

# Test

if (KENGINE_TEST)
    set(test_name kengine_test)
    file(GLOB testSrc
            test/*.cpp test/*.hpp
            test/*/*.cpp test/*/*.hpp
            putils/test/*.cpp putils/test/*.hpp
            putils/test/*/*.cpp putils/test/*/*.hpp
            )
    add_executable(${test_name} ${testSrc})
    target_link_libraries(${test_name} kengine gtest_main)

    if (CMAKE_COMPILER_IS_GNUCXX)
        append_coverage_compiler_flags()
        setup_target_for_coverage(NAME ${test_name}_coverage
                EXECUTABLE ${test_name}
                coverage)
    endif ()
endif ()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} PARENT_SCOPE)
