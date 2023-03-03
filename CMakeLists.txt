cmake_minimum_required(VERSION 3.15)
project(kengine)

cmake_policy(VERSION 3.15) # options shouldn't clear variables

set(CMAKE_CXX_STANDARD 20)

add_library(kengine_include INTERFACE)
target_include_directories(kengine_include INTERFACE .)

if(MSVC)
    target_compile_options(kengine_include INTERFACE /bigobj)
endif()

add_library(kengine INTERFACE)
target_link_libraries(kengine INTERFACE kengine_include)

# Include this here so sub-libraries can use helpers
add_subdirectory(putils/reflection/meta/cmake_helpers kengine_cmake_helpers)

if (NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
    # These must be set before processing sub-libraries, as vcpkg DLLs need to be copied next to executables
    putils_set_output_directories(bin lib lib)
endif()

find_package(EnTT CONFIG REQUIRED)
target_link_libraries(kengine_include INTERFACE EnTT::EnTT)

option(KENGINE_GENERATE_REFLECTION "Generate reflection headers for kengine data types")
option(KENGINE_ALL_LIBRARIES "Build all libraries")
option(KENGINE_TESTS "Build tests")
if(KENGINE_TESTS)
    enable_testing()
endif()

set(
        default_dependencies
        kengine_core
        kengine_core_assert
        kengine_core_log
        kengine_core_profiling
)

function(add_kengine_library path)
    file(RELATIVE_PATH relative_path ${CMAKE_CURRENT_LIST_DIR} ${path})
    # Convert kengine/render/kreogl to kengine_render_kreogl
    string(REPLACE "/" "_" kengine_library_name ${relative_path})

    # Convert kengine_render_kreogl to KENGINE_RENDER_KREOGL
    string(TOUPPER ${kengine_library_name} kengine_library_upper_name)

    # If the plugin isn't one of the default dependencies, add an option to enable it
    list(FIND default_dependencies ${kengine_library_name} default_dependencies_index)
    if(${default_dependencies_index} EQUAL -1)
        set(default_enabled ${KENGINE_ALL_LIBRARIES})
    else()
        set(default_enabled ON)
    endif()

    option(${kengine_library_upper_name} "Build ${kengine_library_name}" ${default_enabled})
    if(NOT ${${kengine_library_upper_name}})
        return()
    endif()

    file(GLOB sources
            ${path}/data/*.cpp
            ${path}/functions/*.cpp
            ${path}/helpers/*.cpp
            ${path}/systems/*.cpp)

    list(LENGTH sources sources_length)
    if(sources_length GREATER 0)
        set(link_type PUBLIC)
        add_library(${kengine_library_name} ${sources})
        putils_export_symbols(${kengine_library_name})
    else()
        set(link_type INTERFACE)
        add_library(${kengine_library_name} INTERFACE)
    endif()

    if(KENGINE_TESTS)
        file(GLOB test_sources
                ${path}/helpers/tests/*.tests.cpp
                ${path}/systems/tests/*.tests.cpp)
        list(LENGTH test_sources test_sources_length)
        if(test_sources_length GREATER 0)
            set(kengine_library_tests_name ${kengine_library_name}_tests)
            putils_add_test_executable(${kengine_library_tests_name} ${test_sources})
            target_link_libraries(${kengine_library_tests_name} PRIVATE ${kengine_library_name})
        endif()
    endif()

    # Expose link type so that the system's CMakeLists can use it
    set(link_type ${link_type} PARENT_SCOPE)

    target_link_libraries(kengine INTERFACE ${kengine_library_name})

    target_compile_definitions(${kengine_library_name} ${link_type} ${kengine_library_upper_name})
    target_link_libraries(${kengine_library_name} ${link_type} kengine_include)

    # Link against the default dependencies
    if(${default_dependencies_index} EQUAL -1)
        target_link_libraries(${kengine_library_name} ${link_type} ${default_dependencies})
    endif()

    if(NOT "${parent_system_name}" STREQUAL "")
        target_link_libraries(${kengine_library_name} ${link_type} ${parent_system_name})
    endif()

    # Headers for which we'll generate reflection headers and/or type registration code
    file(GLOB system_headers
            ${path}/data/*.hpp
            ${path}/functions/*.hpp)

    if(EXISTS ${path}/CMakeLists.txt)
        # Helpers for the system CMakeLists
        macro(subdirectory_is_not_kengine_library path)
            set(ignored_subdirectories ${ignored_subdirectories} ${CMAKE_CURRENT_LIST_DIR}/${path} PARENT_SCOPE)
        endmacro()

        function(kengine_library_link_public_libraries)
            target_link_libraries(${kengine_library_name} ${link_type} ${ARGN})
        endfunction()

        function(kengine_library_link_private_libraries)
            target_link_libraries(${kengine_library_name} PRIVATE ${ARGN})
        endfunction()

        macro(register_types_from_headers)
            foreach(header ${ARGN})
                set(system_headers ${system_headers} ${path}/${header} PARENT_SCOPE)
            endforeach()
        endmacro()
        add_subdirectory(${path})
    endif()

    # Generate reflection headers
    list(LENGTH system_headers headers_length)
    if(headers_length GREATER 0)
        get_target_property(cxx_standard ${kengine_library_name} CXX_STANDARD)
        if(${cxx_standard} STREQUAL cxx_standard-NOTFOUND)
            set_target_properties(${kengine_library_name} PROPERTIES CXX_STANDARD ${CMAKE_CXX_STANDARD})
        endif()

        include(putils/reflection/scripts/generate_reflection_headers.cmake)
        if(KENGINE_GENERATE_REFLECTION)
            putils_generate_reflection_headers(
                    TARGET ${kengine_library_name}
                    SOURCES ${system_headers}
            )
        endif()
    endif()

    set(parent_system_name ${kengine_library_name})
    add_systems_in_directory(${path})

    # Add our headers to the ones we'll want to generate type registration for
    set(headers_to_register ${headers_to_register} ${system_headers} PARENT_SCOPE)
endfunction()

function(add_systems_in_directory root)
    file(GLOB children ${root}/*)
    foreach(child ${children})
        if(NOT IS_DIRECTORY ${child})
            continue()
        endif()

        list(FIND ignored_subdirectories ${child} ignored_index)
        if(NOT ${ignored_index} EQUAL -1)
            continue()
        endif()

        add_kengine_library(${child})
    endforeach()

    set(headers_to_register ${headers_to_register} PARENT_SCOPE)
endfunction()

# Add core first so other systems can link against it
add_kengine_library(${CMAKE_CURRENT_LIST_DIR}/kengine/core)
set(ignored_subdirectories ${CMAKE_CURRENT_LIST_DIR}/kengine/core)

# Then add the other systems
add_systems_in_directory(${CMAKE_CURRENT_LIST_DIR}/kengine)

# putils
add_subdirectory(putils)
target_link_libraries(kengine_include INTERFACE putils)

option(KENGINE_NDEBUG "Disable debug")
if (KENGINE_NDEBUG)
    target_compile_definitions(kengine_include INTERFACE KENGINE_NDEBUG)
endif()

option(KENGINE_TYPE_REGISTRATION "Generate kengine type registration")
if (KENGINE_TYPE_REGISTRATION)
    include(scripts/generate_type_registration.cmake)

    list(LENGTH headers_to_register headers_to_register_length)
    if(headers_to_register_length GREATER 0)
        get_target_property(cxx_standard kengine CXX_STANDARD)
        if(${cxx_standard} STREQUAL cxx_standard-NOTFOUND)
            set_target_properties(kengine PROPERTIES CXX_STANDARD ${CMAKE_CXX_STANDARD})
        endif()

        kengine_generate_type_registration(
                TARGET kengine
                INCLUDE_DIR kengine/type_registration
                SOURCES ${headers_to_register}
                REGISTRATIONS_JSON ${CMAKE_CURRENT_LIST_DIR}/scripts/registrations.json
                NAMESPACE kengine::types
        )
        target_compile_definitions(kengine_type_registration PUBLIC KENGINE_TYPE_REGISTRATION)
        target_link_libraries(kengine_type_registration PUBLIC kengine)
        target_link_libraries(kengine INTERFACE kengine_type_registration)
    endif()
endif()