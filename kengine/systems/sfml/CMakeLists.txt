set(name kengine_sfml)

set(IMGUI_DIR ../../putils/imgui)
file(GLOB src
	*.cpp *.hpp
	imgui-sfml/*.cpp imgui-sfml/*.hpp)

add_library(${name} ${src})
target_link_libraries(${name} PRIVATE kengine_core)
putils_export_symbols(${name})

putils_export_symbols(${name} EXPORT_MACRO_NAME IMGUI_SFML_API EXPORT_FILE_NAME imgui_sfml_api.h)

set(customFindPackageNames sfml:SFML)
set(customLibraryNames sfml:SFML)
putils_conan_set_shared_options(options sfml)
list(APPEND options sfml:audio=False) # doesn't compile in C++20, still uses auto_ptr
putils_conan_download_and_link_packages_with_names(
	${name} PRIVATE
	"${customFindPackageNames}"
	"${customLibraryNames}"
	sfml/2.5.1
	${options}
)

# doesn't compile on mingw with the following error
# cpuid.h:334:1: error: 'void __cpuidex(int*, int, int)' redeclared inline without 'gnu_inline' attribute