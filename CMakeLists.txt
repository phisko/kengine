cmake_minimum_required(VERSION 3.0)
project(kengine)

cmake_policy(VERSION 3.13) # options shouldn't clear variables

set(CMAKE_CXX_STANDARD 20)
if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /DNOMINMAX")
endif()

# System lists

macro(add_system)
    list(APPEND system_options ${ARGV0})
    list(APPEND system_dirs ${ARGV1})
    list(APPEND system_libs ${ARGV2})
endmacro()

add_system(KENGINE_ASSIMP systems/assimp kengine_assimp)
add_system(KENGINE_BULLET systems/bullet kengine_bullet)
add_system(KENGINE_COLLISION systems/collision kengine_collision)
add_system(KENGINE_GLFW systems/glfw kengine_glfw)
add_system(KENGINE_IMGUI_ADJUSTABLE systems/imgui_adjustable kengine_imgui_adjustable)
add_system(KENGINE_IMGUI_ENGINE_STATS systems/imgui_engine_stats kengine_imgui_engine_stats)
add_system(KENGINE_IMGUI_ENTITY_EDITOR systems/imgui_entity_editor kengine_imgui_entity_editor)
add_system(KENGINE_IMGUI_ENTITY_SELECTOR systems/imgui_entity_selector kengine_imgui_entity_selector)
add_system(KENGINE_IMGUI_PROMPT systems/imgui_prompt kengine_imgui_prompt)
add_system(KENGINE_IMGUI_TOOL systems/imgui_tool kengine_imgui_tool)
add_system(KENGINE_INPUT systems/input kengine_input)
add_system(KENGINE_KINEMATIC systems/kinematic kengine_kinematic)
add_system(KENGINE_LOG_FILE systems/log_file kengine_log_file)
add_system(KENGINE_LOG_IMGUI systems/log_imgui kengine_log_imgui)
add_system(KENGINE_LOG_STDOUT systems/log_stdout kengine_log_stdout)
add_system(KENGINE_LOG_VISUAL_STUDIO systems/log_visual_studio kengine_log_visual_studio)
add_system(KENGINE_LUA systems/lua kengine_lua)
add_system(KENGINE_MODEL_CREATOR systems/model_creator kengine_model_creator)
add_system(KENGINE_ONCLICK systems/onclick kengine_onclick)
add_system(KENGINE_OPENGL systems/opengl kengine_opengl)
add_system(KENGINE_OPENGL_SPRITES systems/opengl_sprites kengine_opengl_sprites)
add_system(KENGINE_POLYVOX systems/polyvox kengine_polyvox)
add_system(KENGINE_PYTHON systems/python kengine_python)
add_system(KENGINE_RECAST systems/recast kengine_recast)
add_system(KENGINE_SFML systems/sfml kengine_sfml)

list(LENGTH system_options system_count)
math(EXPR system_count "${system_count} - 1")

# Options

option(KENGINE_ALL_SYSTEMS "Build all systems" OFF)

foreach(i RANGE ${system_count})
    list(GET system_options ${i} sys_option)
    list(GET system_libs ${i} sys_lib)
    option(${sys_option} "Build ${sys_lib}" OFF)
    if (KENGINE_ALL_SYSTEMS)
        set(${sys_option} ON)
    endif()
endforeach()

option(KENGINE_TESTS "Build tests" OFF)
option(KENGINE_NDEBUG "Disable debug" OFF)

# putils

set(PUTILS_TESTS ${KENGINE_TESTS})
set(PUTILS_NDEBUG ${KENGINE_NDEBUG})

set(PUTILS_PSE ${KENGINE_SFML})
set(PUTILS_LUA ${KENGINE_LUA})
set(PUTILS_PYTHON ${KENGINE_PYTHON})
set(PUTILS_OPENGL ${KENGINE_OPENGL})

set(PUTILS_IMGUI ON)
set(PUTILS_PLUGIN_MANAGER ON)
add_subdirectory(putils)

# Library

putils_src_files(src_files
	.
	impl
	components/data
	components/functions
	components/meta
	helpers
	helpers/meta
)

option(KENGINE_NO_TYPE_REGISTRATION "Do not register kengine types" OFF)
if (NOT KENGINE_NO_TYPE_REGISTRATION)
    file(GLOB type_files impl/types/*.cpp impl/types/*.hpp)
    set(src_files ${src_files} ${type_files})
endif()

add_library(kengine STATIC ${src_files})
target_link_libraries(kengine PUBLIC putils)

if (KENGINE_NDEBUG)
    target_compile_definitions(kengine PUBLIC KENGINE_NDEBUG)
endif()

target_include_directories(kengine PUBLIC . components)

if (MSVC)
    if (NOT KENGINE_NDEBUG)
        target_link_libraries(kengine PUBLIC Dbghelp)
    endif()
endif()

if (KENGINE_NO_TYPE_REGISTRATION)
    target_compile_definitions(kengine PUBLIC KENGINE_NO_TYPE_REGISTRATION)
endif()

# Add systems

foreach(i RANGE ${system_count})
    list(GET system_options ${i} sys_option)
    list(GET system_dirs ${i} sys_dir)
    list(GET system_libs ${i} sys_lib)

    if (${sys_option})
        target_compile_definitions(kengine PUBLIC ${sys_option})
        add_subdirectory(${sys_dir})
        target_link_libraries(kengine PUBLIC ${sys_lib})
    endif()
endforeach()

#
# Tests
#

if (KENGINE_TESTS)
    set(test_exe_name kengine_tests)

    file(GLOB test_src
            tests/*.tests.cpp
            helpers/tests/*.tests.cpp
            impl/tests/*.tests.cpp)

    add_executable(${test_exe_name} ${test_src})

    putils_conan(gtest/cci.20210126)
    target_link_libraries(${test_exe_name} CONAN_PKG::gtest kengine)

    include(GoogleTest)
    gtest_discover_tests(${test_exe_name})
endif()
