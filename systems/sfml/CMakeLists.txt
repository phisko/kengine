set(name kengine_sfml)

set(IMGUI_DIR ../../putils/imgui)
file(GLOB src
	*.cpp *.hpp
	imgui-sfml/*.cpp imgui-sfml/*.hpp)

add_library(${name} STATIC ${src})

putils_conan(sfml/2.5.1) # doesn't compile on mingw with the following error
# cpuid.h:334:1: error: 'void __cpuidex(int*, int, int)' redeclared inline without 'gnu_inline' attribute
target_link_libraries(${name} kengine CONAN_PKG::sfml)
