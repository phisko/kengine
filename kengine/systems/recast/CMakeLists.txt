set(name kengine_recast)

file(GLOB src *.cpp *.hpp)
add_library(${name} ${src})
target_link_libraries(${name} PRIVATE kengine_core)
putils_export_symbols(${name})

set(customFindPackageNames)
set(customLibraryNames recastnavigation:RecastNavigation)
putils_conan_set_shared_options(options recastnavigation)
putils_conan_download_and_link_packages_with_names(
	${name} PRIVATE
	"${customFindPackageNames}"
	"${customLibraryNames}"
	recastnavigation/cci.20200511
	${options}
)