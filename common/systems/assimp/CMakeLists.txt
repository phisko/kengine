set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
add_subdirectory(libs/assimp)

set(name kengine_assimp)

file(GLOB src *.cpp *.hpp)
add_library(${name} STATIC ${src})
target_link_libraries(${name} PUBLIC kengine assimp)
target_include_directories(${name} PRIVATE ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/libs/assimp/include
    ${CMAKE_CURRENT_BINARY_DIR}/libs/assimp/include
)
