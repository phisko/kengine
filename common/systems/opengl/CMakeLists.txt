set(name kengine_opengl)

set(BUILD_UTILS FALSE)
add_subdirectory(libs/glew/build/cmake)

set(GLFW_BUILD_EXAMPLES FALSE)
set(GLFW_BUILD_TESTS FALSE)
add_subdirectory(libs/glfw)

add_subdirectory(libs/glm/glm)

file(GLOB src
	libs/soil/src/*
	*.cpp *.hpp
    shaders/*.cpp shaders/*.hpp
)

if (UNIX)
    set(type SHARED)
elseif (WIN32)
    set(type STATIC)
endif ()

add_library(${name} ${type} ${src})

set(ADDITIONAL_LIBS ${ADDITIONAL_LIBS} glfw glew glm_static)
set(ADDITIONAL_LIBS ${ADDITIONAL_LIBS} glfw glew glm_static PARENT_SCOPE)

set(ADDITIONAL_INCLUDE_PATHS ${ADDITIONAL_INCLUDE_PATHS}
	${CMAKE_CURRENT_LIST_DIR}/libs/polyvox/include
    ${CMAKE_CURRENT_LIST_DIR}/libs/glew/include
	${CMAKE_CURRENT_LIST_DIR}/libs/glm
)
set(ADDITIONAL_INCLUDE_PATHS ${ADDITIONAL_INCLUDE_PATHS}
	${CMAKE_CURRENT_LIST_DIR}/libs/polyvox/include
    ${CMAKE_CURRENT_LIST_DIR}/libs/glew/include
	${CMAKE_CURRENT_LIST_DIR}/libs/glm
	PARENT_SCOPE
)

set(IMGUI_DIR ../../../putils/pse/imgui)
add_library(imgui ${type}
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/examples/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/examples/imgui_impl_opengl3.cpp
)
target_compile_definitions(imgui PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLEW)
target_include_directories(imgui PUBLIC ${IMGUI_DIR} ${ADDITIONAL_INCLUDE_PATHS})
target_link_libraries(imgui glfw glew)

target_link_libraries(${name} PRIVATE
	kengine pluginManager imgui
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
