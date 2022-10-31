set(name kengine_sfml)

set(IMGUI_DIR ../../putils/imgui)
file(GLOB src
	*.cpp *.hpp
	imgui-sfml/*.cpp imgui-sfml/*.hpp)

add_library(${name} STATIC ${src})
target_link_libraries(${name} PRIVATE kengine)

set(customFindPackageNames sfml:SFML)
set(customLibraryNames sfml:SFML)
putils_conan_download_and_link_packages_with_names(
	${name} PRIVATE
	"${customFindPackageNames}"
	"${customLibraryNames}"
	sfml/2.5.1
	OPTIONS
		sfml:audio=False # doesn't compile in C++20, still uses auto_ptr
)

# doesn't compile on mingw with the following error
# cpuid.h:334:1: error: 'void __cpuidex(int*, int, int)' redeclared inline without 'gnu_inline' attribute