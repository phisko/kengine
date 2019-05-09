#
# SFML System
#

set(ASSIMP_BUILD_ASSIMP_TOOLS FALSE)
set(ASSIMP_BUILD_TESTS FALSE)
add_subdirectory(libs/assimp)

set(name kengine_assimp)

file(GLOB src *.cpp)

if (UNIX)
    set(type SHARED)
elseif (WIN32)
    set(type STATIC)
endif ()

add_library(${name} ${type} ${src})
target_link_libraries(${name} PUBLIC kengine imgui assimp)
target_include_directories(${name} PRIVATE ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/libs/assimp/include
    ${CMAKE_CURRENT_BINARY_DIR}/libs/assimp/include
)
