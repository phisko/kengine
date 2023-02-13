project(kengine)

set(customFindPackageNames glfw:glfw3)
set(customLibraryNames)
putils_conan_set_shared_options(options glfw)
putils_conan_download_and_link_packages_with_names(
	${system_name} PUBLIC
	"${customFindPackageNames}"
	"${customLibraryNames}"
	glfw/3.3.6
	${options})