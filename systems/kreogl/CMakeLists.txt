set(name kengine_kreogl)

set(KREOGL_DEFAULT_SHADERS ON)
add_subdirectory(kreogl)

set(imguiDir ${PROJECT_SOURCE_DIR}/putils/imgui/)
file(GLOB src
	*.cpp *.hpp
	shaders/*.cpp shaders/*.hpp
	${imguiDir}/backends/imgui_impl_glfw.cpp ${imguiDir}/backends/imgui_impl_opengl3.cpp)
add_library(${name} STATIC ${src})
target_link_libraries(${name} PRIVATE kengine kreogl)