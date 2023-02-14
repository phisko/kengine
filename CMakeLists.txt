cmake_minimum_required(VERSION 3.8)
project(kengine)

cmake_policy(VERSION 3.13) # options shouldn't clear variables

set(CMAKE_CXX_STANDARD 20)
if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /DNOMINMAX")
endif()

add_library(kengine_include INTERFACE)
target_include_directories(kengine_include INTERFACE .)

add_library(kengine INTERFACE)
target_link_libraries(kengine INTERFACE kengine_include)

# Include this here so systems can use helpers
add_subdirectory(putils/reflection/meta/cmake_helpers kengine_cmake_helpers)

if (NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
    # These must be set before processing systems, as conan DLLs need to be copied next to executables
    putils_set_output_directories(bin lib lib)
endif()

# entt
set(customFindPackageNames entt:EnTT)
set(customLibraryNames entt:EnTT)
putils_conan_download_and_link_packages_with_names(
        kengine_include INTERFACE
        "${customFindPackageNames}"
        "${customLibraryNames}"
        entt/3.11.0
)

option(KENGINE_GENERATE_REFLECTION "Generate reflection headers for kengine data types")
option(KENGINE_ALL_SYSTEMS "Build all systems")

function(add_system path)
    file(RELATIVE_PATH relative_path ${CMAKE_CURRENT_LIST_DIR} ${path})
    # Convert kengine/render/kreogl to kengine_render_kreogl
    string(REPLACE "/" "_" system_name ${relative_path})

    # Convert kengine_render_kreogl to KENGINE_RENDER_KREOGL
    string(TOUPPER ${system_name} upper_system_name)

    option(${upper_system_name} "Build ${system_name}" ${KENGINE_ALL_SYSTEMS})
    if(NOT ${${upper_system_name}})
        return()
    endif()

    file(GLOB sources
            ${path}/helpers/*.cpp
            ${path}/systems/*.cpp)

    list(LENGTH sources sources_length)
    if(sources_length GREATER 0)
        set(link_type PUBLIC)
        add_library(${system_name} ${sources})
        putils_export_symbols(${system_name})
    else()
        set(link_type INTERFACE)
        add_library(${system_name} INTERFACE)
    endif()

    # Expose link type so that the system's CMakeLists can use it
    set(link_type ${link_type} PARENT_SCOPE)

    target_link_libraries(kengine INTERFACE ${system_name})

    target_compile_definitions(${system_name} ${link_type} ${upper_system_name})
    target_link_libraries(${system_name} ${link_type} kengine_include)
    if(NOT ${system_name} STREQUAL kengine_core)
        if(TARGET kengine_core)
            target_link_libraries(${system_name} ${link_type} kengine_core)
        endif()
    endif()

    if(NOT "${parent_system_name}" STREQUAL "")
        target_link_libraries(${system_name} ${link_type} ${parent_system_name})
    endif()

    # Headers for which we'll generate reflection headers and/or type registration code
    file(GLOB system_headers
            ${path}/data/*.hpp
            ${path}/functions/*.hpp)

    if(EXISTS ${path}/CMakeLists.txt)
        # Helpers for the system CMakeLists
        macro(subdirectory_is_not_system path)
            set(ignored_subdirectories ${ignored_subdirectories} ${CMAKE_CURRENT_LIST_DIR}/${path} PARENT_SCOPE)
        endmacro()

        function(system_link_public_libraries)
            target_link_libraries(${system_name} ${link_type} ${ARGN})
        endfunction()

        function(system_link_private_libraries)
            target_link_libraries(${system_name} PRIVATE ${ARGN})
        endfunction()

        macro(register_types_from_headers)
            set(system_headers ${system_headers} ${ARGN} PARENT_SCOPE)
        endmacro()
        add_subdirectory(${path})
    endif()

    # Generate reflection headers
    list(LENGTH system_headers headers_length)
    if(headers_length GREATER 0)
        get_target_property(cxx_standard ${system_name} CXX_STANDARD)
        if(${cxx_standard} STREQUAL cxx_standard-NOTFOUND)
            set_target_properties(${system_name} PROPERTIES CXX_STANDARD ${CMAKE_CXX_STANDARD})
        endif()

        include(putils/reflection/scripts/generate_reflection_headers.cmake)
        if(KENGINE_GENERATE_REFLECTION)
            putils_generate_reflection_headers(
                    TARGET ${system_name}
                    SOURCES ${system_headers}
            )
        endif()
    endif()

    set(parent_system_name ${system_name})
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

        add_system(${child})
    endforeach()

    set(headers_to_register ${headers_to_register} ${system_headers} PARENT_SCOPE)
endfunction()

# Add core first so other systems can link against it
add_system(${CMAKE_CURRENT_LIST_DIR}/kengine/core)
set(ignored_subdirectories ${CMAKE_CURRENT_LIST_DIR}/kengine/core)

# Then add the other systems
add_systems_in_directory(kengine)

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