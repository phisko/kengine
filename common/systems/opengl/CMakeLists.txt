set(name opengl)

set(BUILD_UTILS FALSE)
add_subdirectory(libs/glew/build/cmake)

set(GLFW_BUILD_EXAMPLES FALSE)
set(GLFW_BUILD_TESTS FALSE)
add_subdirectory(libs/glfw)

add_subdirectory(libs/glm/glm)

set(ASSIMP_BUILD_ASSIMP_TOOLS FALSE)
set(ASSIMP_BUILD_TESTS FALSE)
add_subdirectory(libs/assimp)

file(GLOB src
	libs/soil/src/*
	*.cpp *.hpp
    shaders/*.cpp shaders/*.hpp
)

set(IMGUI_DIR ../../../putils/pse/imgui)
add_library(${name} SHARED MODULE ${src}
    ${IMGUI_DIR}/examples/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/examples/imgui_impl_opengl3.cpp
)
target_compile_definitions(${name} PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLEW)

set(ADDITIONAL_LIBS ${ADDITIONAL_LIBS} glfw glew glm_static assimp)
set(ADDITIONAL_LIBS ${ADDITIONAL_LIBS} glfw glew glm_static assimp PARENT_SCOPE)

set(ADDITIONAL_INCLUDE_PATHS ${ADDITIONAL_INCLUDE_PATHS}
	${CMAKE_CURRENT_LIST_DIR}/libs/polyvox/include
    ${CMAKE_CURRENT_LIST_DIR}/libs/glew/include
	${CMAKE_CURRENT_LIST_DIR}/libs/glm
    ${CMAKE_CURRENT_LIST_DIR}/libs/assimp/include
    ${CMAKE_CURRENT_BINARY_DIR}/libs/assimp/include
)
set(ADDITIONAL_INCLUDE_PATHS ${ADDITIONAL_INCLUDE_PATHS}
	${CMAKE_CURRENT_LIST_DIR}/libs/polyvox/include
    ${CMAKE_CURRENT_LIST_DIR}/libs/glew/include
	${CMAKE_CURRENT_LIST_DIR}/libs/glm
    ${CMAKE_CURRENT_LIST_DIR}/libs/assimp/include
    ${CMAKE_CURRENT_LIST_DIR}/libs/assimp/include
	PARENT_SCOPE
)

target_link_libraries(${name} PRIVATE
	kengine pluginManager api
	${ADDITIONAL_LIBS}
)

target_include_directories(${name} PRIVATE
	${CMAKE_CURRENT_LIST_DIR}
	${ADDITIONAL_INCLUDE_PATHS}
)

## Copy files

add_custom_command(TARGET ${name} PRE_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
	${CMAKE_CURRENT_LIST_DIR}/shaders ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/shaders
)
add_custom_command(TARGET ${name} PRE_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
	${CMAKE_CURRENT_LIST_DIR}/shaders ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}/shaders
)
add_custom_command(TARGET ${name} PRE_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
	${CMAKE_CURRENT_LIST_DIR}/shaders ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}/shaders
)
add_custom_command(TARGET ${name} PRE_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
	${CMAKE_CURRENT_LIST_DIR}/shaders ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO}/shaders
)
add_custom_command(TARGET ${name} PRE_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_directory
	${CMAKE_CURRENT_LIST_DIR}/shaders ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL}/shaders
)
