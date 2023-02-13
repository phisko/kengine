cmake_minimum_required(VERSION 3.8)
project(kengine)

cmake_policy(VERSION 3.13) # options shouldn't clear variables

set(CMAKE_CXX_STANDARD 20)
if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /DNOMINMAX")
endif()

add_library(kengine INTERFACE)

# Include this here so systems can use helpers
add_subdirectory(putils/reflection/meta/cmake_helpers kengine_cmake_helpers)
if (NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
    # These must be set before processing systems, as conan DLLs need to be copied next to executables
    putils_set_output_directories(bin lib lib)
endif()

option(KENGINE_GENERATE_REFLECTION "Generate reflection headers for kengine data types")
option(KENGINE_ALL_SYSTEMS "Build all systems")

function(add_system path)
    file(RELATIVE_PATH relative_path ${CMAKE_CURRENT_LIST_DIR} ${path})
    # Convert kengine/render/kreogl to kengine_render_kreogl
    string(REPLACE "/" "_" system_name ${relative_path})

    # Expose system name to parent scope so that the system's CMakeLists can use it
    set(system_name ${system_name} PARENT_SCOPE)

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
    if(NOT ${system_name} STREQUAL kengine_core)
        target_link_libraries(${system_name} ${link_type} kengine_core)
    endif()

    if(NOT "${parent_system_name}" STREQUAL "")
        target_link_libraries(${system_name} ${link_type} ${parent_system_name})
    endif()

    file(GLOB types
            ${path}/data/*.hpp
            ${path}/functions/*.hpp)

    list(LENGTH types types_length)
    if(types_length GREATER 0)
        get_target_property(cxx_standard ${system_name} CXX_STANDARD)
        if(${cxx_standard} STREQUAL cxx_standard-NOTFOUND)
            set_target_properties(${system_name} PROPERTIES CXX_STANDARD ${CMAKE_CXX_STANDARD})
        endif()

        include(putils/reflection/scripts/generate_reflection_headers.cmake)
        if(${KENGINE_GENERATE_REFLECTION})
            putils_generate_reflection_headers(
                    TARGET ${system_name}
                    SOURCES ${types}
            )
        endif()
    endif()

    set(types_to_register ${types_to_register} ${types} PARENT_SCOPE)
endfunction()

function(recursively_add_subdirectories root)
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
        if(EXISTS ${child}/CMakeLists.txt)
            macro(subdirectory_is_not_system path)
                set(ignored_subdirectories ${ignored_subdirectories} ${CMAKE_CURRENT_LIST_DIR}/${path} PARENT_SCOPE)
            endmacro()
            function(system_link_public_libraries)
                target_link_libraries(${system_name} ${link_type} ${ARGN})
            endfunction()
            function(system_link_private_libraries)
                target_link_libraries(${system_name} PRIVATE ${ARGN})
            endfunction()
            add_subdirectory(${child})
        endif()

        set(previous_parent_system_name ${parent_system_name})
        set(parent_system_name ${system_name})
        recursively_add_subdirectories(${child})
        set(parent_system_name ${previous_parent_system_name})
    endforeach()

    set(types_to_register ${types_to_register} ${types} PARENT_SCOPE)
endfunction()

# Add core first so other systems can link against it
add_system(${CMAKE_CURRENT_LIST_DIR}/kengine/core)
set(ignored_subdirectories ${CMAKE_CURRENT_LIST_DIR}/kengine/core)

target_include_directories(kengine_core PUBLIC .)
target_link_libraries(kengine INTERFACE kengine_core)

# entt
set(customFindPackageNames entt:EnTT)
set(customLibraryNames entt:EnTT)
putils_conan_download_and_link_packages_with_names(
        kengine_core PUBLIC
        "${customFindPackageNames}"
        "${customLibraryNames}"
        entt/3.11.0
)

# Then add the other systems
recursively_add_subdirectories(kengine)

# putils
add_subdirectory(putils)
target_link_libraries(kengine_core PUBLIC putils)

option(KENGINE_NDEBUG "Disable debug")
if (KENGINE_NDEBUG)
    target_compile_definitions(kengine_core PUBLIC KENGINE_NDEBUG)
endif()

option(KENGINE_TYPE_REGISTRATION "Generate kengine type registration")
if (KENGINE_TYPE_REGISTRATION)
    include(scripts/generate_type_registration.cmake)

    list(LENGTH types_to_register types_to_register_length)
    if(types_to_register_length GREATER 0)
        get_target_property(cxx_standard kengine CXX_STANDARD)
        if(${cxx_standard} STREQUAL cxx_standard-NOTFOUND)
            set_target_properties(kengine PROPERTIES CXX_STANDARD ${CMAKE_CXX_STANDARD})
        endif()

        kengine_generate_type_registration(
                TARGET kengine
                INCLUDE_DIR kengine/type_registration
                SOURCES ${types_to_register}
                REGISTRATIONS_JSON ${CMAKE_CURRENT_LIST_DIR}/scripts/registrations.json
                NAMESPACE kengine::types
        )
        target_compile_definitions(kengine_type_registration PUBLIC KENGINE_TYPE_REGISTRATION)
        target_link_libraries(kengine_type_registration PUBLIC kengine)
        target_link_libraries(kengine INTERFACE kengine_type_registration)
    endif()
endif()
