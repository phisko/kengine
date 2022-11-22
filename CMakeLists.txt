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

add_system(KENGINE_BULLET systems/bullet kengine_bullet KENGINE_GLM)
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
add_system(KENGINE_KREOGL systems/kreogl kengine_kreogl KENGINE_NO_DEPENDENCY)
add_system(KENGINE_LOG_FILE systems/log_file kengine_log_file KENGINE_NO_DEPENDENCY)
add_system(KENGINE_LOG_IMGUI systems/log_imgui kengine_log_imgui KENGINE_NO_DEPENDENCY)
add_system(KENGINE_LOG_STDOUT systems/log_stdout kengine_log_stdout KENGINE_NO_DEPENDENCY)
add_system(KENGINE_LOG_VISUAL_STUDIO systems/log_visual_studio kengine_log_visual_studio KENGINE_NO_DEPENDENCY)
add_system(KENGINE_LUA systems/lua kengine_lua PUTILS_LUA)
add_system(KENGINE_MODEL_CREATOR systems/model_creator kengine_model_creator KENGINE_NO_DEPENDENCY)
add_system(KENGINE_ONCLICK systems/onclick kengine_onclick KENGINE_NO_DEPENDENCY)
add_system(KENGINE_POLYVOX systems/polyvox kengine_polyvox KENGINE_NO_DEPENDENCY)
add_system(KENGINE_PYTHON systems/python kengine_python PUTILS_PYTHON)
add_system(KENGINE_RECAST systems/recast kengine_recast KENGINE_GLM)
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

if (NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
    # These must be set before processing putils, as conan DLLs need to be copied next to executables
    list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/putils/reflection/meta/cmake_helpers/CMakeModules)
    include(putils_set_output_directories)
    putils_set_output_directories(bin lib lib)
endif()

# putils

option(PUTILS_PLUGIN_MANAGER "Build PluginManager" OFF)
set(PUTILS_PLUGIN_MANAGER ON)

option(KENGINE_PROFILING OFF)
option(PUTILS_PROFILING OFF)
set(PUTILS_PROFILING ${KENGINE_PROFILING})

add_subdirectory(putils)

# Library

add_library(kengine INTERFACE)

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

add_library(kengine_core ${src_files})
target_link_libraries(kengine_core PUBLIC putils)
target_include_directories(kengine_core PUBLIC . components)
putils_export_symbols(kengine_core)

set(customFindPackageNames entt:EnTT)
set(customLibraryNames entt:EnTT)
putils_conan_download_and_link_packages_with_names(
    kengine_core PUBLIC
    "${customFindPackageNames}"
    "${customLibraryNames}"
    entt/3.11.0
)

if (KENGINE_PROFILING)
    target_compile_definitions(kengine_core PUBLIC KENGINE_PROFILING)
endif()

if (KENGINE_GLM)
    target_compile_definitions(kengine_core PUBLIC KENGINE_GLM)
    putils_conan_download_and_link_packages(kengine_core PUBLIC glm/0.9.9.8)
endif()

option(KENGINE_NDEBUG "Disable debug" OFF)
if (KENGINE_NDEBUG)
    target_compile_definitions(kengine_core PUBLIC KENGINE_NDEBUG)
endif()

if (KENGINE_TYPE_REGISTRATION)
    target_compile_definitions(kengine_core PUBLIC KENGINE_TYPE_REGISTRATION)
endif()

target_link_libraries(kengine INTERFACE kengine_core)

# Add systems

foreach(i RANGE ${system_count})
    list(GET system_options ${i} sys_option)
    list(GET system_dirs ${i} sys_dir)
    list(GET system_libs ${i} sys_lib)

    if (${sys_option})
        # Add definition to core, so that helpers which depend on a library get compiled
        # TODO: move those helpers into the associated systems and only add the definitions to them
        target_compile_definitions(kengine_core PUBLIC ${sys_option})

        add_subdirectory(${sys_dir})
        target_link_libraries(kengine INTERFACE ${sys_lib})
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

    putils_add_test_executable(${test_exe_name} ${test_src})
    target_link_libraries(${test_exe_name} PRIVATE kengine_core)
endif()
