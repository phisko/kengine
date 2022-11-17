set(name kengine_kreogl)

set(KREOGL_PROFILING ${KENGINE_PROFILING})
add_subdirectory(kreogl)

file(GLOB src
	*.cpp *.hpp
	shaders/*.cpp shaders/*.hpp)

# ImGui bindings
get_target_property(putilsDir putils BINARY_DIR)
set(imguiResDir ${putilsDir}/imgui_res)
list(APPEND src
		${imguiResDir}/bindings/imgui_impl_opengl3.cpp
		${imguiResDir}/bindings/imgui_impl_glfw.cpp)

add_library(${name} ${src})
target_link_libraries(${name} PRIVATE kengine kreogl)
target_include_directories(${name} PRIVATE ${imguiResDir})

putils_export_symbols(${name})
putils_export_symbols(${name}
		EXPORT_MACRO_NAME IMGUI_IMPL_API
		EXPORT_FILE_NAME imgui_impl_api.h)