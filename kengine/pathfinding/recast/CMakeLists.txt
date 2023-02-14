project(kengine)

system_link_private_libraries(kengine_glm)

set(customFindPackageNames)
set(customLibraryNames recastnavigation:RecastNavigation)
putils_conan_set_shared_options(options recastnavigation)
putils_conan_download_and_link_packages_with_names(
	${system_name} PUBLIC
	"${customFindPackageNames}"
	"${customLibraryNames}"
	recastnavigation/cci.20200511
	${options}
)