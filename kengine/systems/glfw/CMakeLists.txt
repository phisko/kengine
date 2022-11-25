set(name kengine_glfw)

file(GLOB src *.cpp *.hpp)

add_library(${name} ${src})
target_link_libraries(${name} PRIVATE kengine_core)
putils_export_symbols(${name})

set(customFindPackageNames glfw:glfw3)
set(customLibraryNames)
putils_conan_set_shared_options(options glfw)
putils_conan_download_and_link_packages_with_names(
	${name} PUBLIC
	"${customFindPackageNames}"
	"${customLibraryNames}"
	glfw/3.3.6
	${options})