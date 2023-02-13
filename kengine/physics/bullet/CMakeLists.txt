project(kengine)

target_link_libraries(
		${system_name} PRIVATE
		kengine_glm
		kengine_skeleton
)

set(customFindPackageNames bullet3:Bullet)
set(customLibraryNames bullet3:Bullet)
if (NOT MSVC) # bullet3/3.21: Invalid ID: Shared libraries on Visual Studio not supported
	putils_conan_set_shared_options(options bullet3)
endif()
putils_conan_download_and_link_packages_with_names(
	${system_name} PRIVATE
	"${customFindPackageNames}"
	"${customLibraryNames}"
	bullet3/3.21
	${options}
)