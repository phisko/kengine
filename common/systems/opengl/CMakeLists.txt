set(name kengine_opengl)

add_subdirectory(libs/freetype)

set(IMGUI_DIR ../../../putils/imgui)
file(GLOB src
	libs/soil/src/*.cpp
	*.cpp *.hpp
	shaders/*.cpp shaders/*.hpp
	${IMGUI_DIR}/examples/imgui_impl_glfw.cpp
	${IMGUI_DIR}/examples/imgui_impl_opengl3.cpp
)

add_library(${name} STATIC ${src})

target_compile_definitions(${name} PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLEW)
target_include_directories(${name} PUBLIC
	${CMAKE_CURRENT_LIST_DIR}
	${CMAKE_CURRENT_LIST_DIR}/libs/glew/include
)

target_link_libraries(${name} kengine glfw glew freetype)
