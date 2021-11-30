set(name kengine_glfw)

file(GLOB src *.cpp *.hpp)

add_library(${name} STATIC ${src})

putils_conan(
	glfw/3.3@bincrafters/stable
	OPTIONS glfw:shared=True
)
target_link_libraries(${name} kengine CONAN_PKG::glfw)