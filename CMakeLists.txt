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
    list(APPEND system_dependent_options ${ARGV3})
endmacro()

add_system(KENGINE_BULLET systems/bullet kengine_bullet KENGINE_NO_DEPENDENCY)
add_system(KENGINE_COLLISION systems/collision kengine_collision KENGINE_NO_DEPENDENCY)
add_system(KENGINE_GLFW systems/glfw kengine_glfw KENGINE_NO_DEPENDENCY)
add_system(KENGINE_IMGUI_ADJUSTABLE systems/imgui_adjustable kengine_imgui_adjustable KENGINE_NO_DEPENDENCY)
add_system(KENGINE_IMGUI_ENGINE_STATS systems/imgui_engine_stats kengine_imgui_engine_stats KENGINE_NO_DEPENDENCY)
add_system(KENGINE_IMGUI_ENTITY_EDITOR systems/imgui_entity_editor kengine_imgui_entity_editor KENGINE_NO_DEPENDENCY)
add_system(KENGINE_IMGUI_ENTITY_SELECTOR systems/imgui_entity_selector kengine_imgui_entity_selector KENGINE_NO_DEPENDENCY)
add_system(KENGINE_IMGUI_PROMPT systems/imgui_prompt kengine_imgui_prompt KENGINE_NO_DEPENDENCY)
add_system(KENGINE_IMGUI_TOOL systems/imgui_tool kengine_imgui_tool KENGINE_NO_DEPENDENCY)
add_system(KENGINE_INPUT systems/input kengine_input KENGINE_NO_DEPENDENCY)
add_system(KENGINE_KINEMATIC systems/kinematic kengine_kinematic KENGINE_NO_DEPENDENCY)
add_system(KENGINE_LOG_FILE systems/log_file kengine_log_file KENGINE_NO_DEPENDENCY)
add_system(KENGINE_LOG_IMGUI systems/log_imgui kengine_log_imgui KENGINE_NO_DEPENDENCY)
add_system(KENGINE_LOG_STDOUT systems/log_stdout kengine_log_stdout KENGINE_NO_DEPENDENCY)
add_system(KENGINE_LOG_VISUAL_STUDIO systems/log_visual_studio kengine_log_visual_studio KENGINE_NO_DEPENDENCY)
add_system(KENGINE_LUA systems/lua kengine_lua PUTILS_LUA)
add_system(KENGINE_MODEL_CREATOR systems/model_creator kengine_model_creator KENGINE_NO_DEPENDENCY)
add_system(KENGINE_ONCLICK systems/onclick kengine_onclick KENGINE_NO_DEPENDENCY)
add_system(KENGINE_POLYVOX systems/polyvox kengine_polyvox KENGINE_NO_DEPENDENCY)
add_system(KENGINE_PYTHON systems/python kengine_python PUTILS_PYTHON)
add_system(KENGINE_RECAST systems/recast kengine_recast KENGINE_NO_DEPENDENCY)
add_system(KENGINE_SFML systems/sfml kengine_sfml PUTILS_PSE)

list(LENGTH system_options system_count)
math(EXPR system_count "${system_count} - 1")

# Options

option(KENGINE_ALL_SYSTEMS "Build all systems" OFF)

foreach(i RANGE ${system_count})
    list(GET system_options ${i} sys_option)
    list(GET system_libs ${i} sys_lib)
    list(GET system_dependent_options ${i} sys_dependent_options)

    option(${sys_option} "Build ${sys_lib}" OFF)
    if (KENGINE_ALL_SYSTEMS)
        set(${sys_option} ON)
    endif()

    foreach(opt in ${sys_dependent_options})
		if(${sys_option})
			option(${opt} OFF)
			set(${opt} ON)
		endif()
    endforeach()
endforeach()

# putils

option(PUTILS_IMGUI "Build ImGui" OFF)
set(PUTILS_IMGUI ON)

option(PUTILS_PLUGIN_MANAGER "Build PluginManager" OFF)
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

option(KENGINE_TYPE_REGISTRATION "Automatically register kengine types" OFF)
if (KENGINE_TYPE_REGISTRATION)
    file(GLOB type_files impl/types/*.cpp impl/types/*.hpp)
    set(src_files ${src_files} ${type_files})
endif()

add_library(kengine STATIC ${src_files})
target_link_libraries(kengine PUBLIC putils)

option(KENGINE_NDEBUG "Disable debug" OFF)
if (KENGINE_NDEBUG)
    target_compile_definitions(kengine PUBLIC KENGINE_NDEBUG)
endif()

target_include_directories(kengine PUBLIC . components)

if (MSVC)
    if (NOT KENGINE_NDEBUG)
        target_link_libraries(kengine PUBLIC Dbghelp)
    endif()
endif()

if (KENGINE_TYPE_REGISTRATION)
    target_compile_definitions(kengine PUBLIC KENGINE_TYPE_REGISTRATION)
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

option(KENGINE_TESTS "Build kengine tests" OFF)
if (KENGINE_TESTS)
    enable_testing()
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
