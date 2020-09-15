set(name kengine_opengl)

set(IMGUI_DIR ../../putils/imgui)
file(GLOB src
	*.cpp *.hpp
	${IMGUI_DIR}/examples/imgui_impl_glfw.cpp
	${IMGUI_DIR}/examples/imgui_impl_opengl3.cpp
)

if (KENGINE_OPENGL_NO_DEBUG_TOOLS)
	add_compile_definitions(KENGINE_OPENGL_NO_DEBUG_TOOLS)
endif()

if (KENGINE_OPENGL_NO_DEFAULT_SHADERS)
	add_compile_definitions(KENGINE_OPENGL_NO_DEFAULT_SHADERS)
else()
	file(GLOB shadersSrc shaders/*.cpp shaders/*.hpp)
	set(src ${src} ${shadersSrc})
endif()

add_library(${name} STATIC ${src})

putils_conan(
	freetype/2.10.0@bincrafters/stable
	glfw/3.3@bincrafters/stable
	glm/0.9.9.5@g-truc/stable
	OPTIONS glfw:shared=True
)
target_link_libraries(${name} kengine CONAN_PKG::glfw CONAN_PKG::freetype CONAN_PKG::glm)