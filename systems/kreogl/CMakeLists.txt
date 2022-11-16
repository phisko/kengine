set(name kengine_kreogl)

set(KREOGL_DEFAULT_SHADERS ON)
set(KREOGL_PROFILING ${KENGINE_PROFILING})
add_subdirectory(kreogl)

set(imguiDir ${PROJECT_SOURCE_DIR}/putils/imgui/)
file(GLOB src
	*.cpp *.hpp
	shaders/*.cpp shaders/*.hpp
	${imguiDir}/backends/imgui_impl_glfw.cpp ${imguiDir}/backends/imgui_impl_opengl3.cpp)
add_library(${name} ${src})
target_link_libraries(${name} PRIVATE kengine kreogl)
putils_export_symbols(${name})
putils_export_symbols(${name} EXPORT_MACRO_NAME IMGUI_IMPL_API EXPORT_FILE_NAME imgui_impl_api.h)