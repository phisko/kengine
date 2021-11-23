set(name kengine_sfml)

set(IMGUI_DIR ../../putils/imgui)
file(GLOB src
	*.cpp *.hpp
	imgui-sfml/*.cpp imgui-sfml/*.hpp)

add_library(${name} STATIC ${src})

putils_conan(sfml/2.5.1)
target_link_libraries(${name} kengine CONAN_PKG::sfml)
