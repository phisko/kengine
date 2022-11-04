set(name kengine_recast)

file(GLOB src *.cpp *.hpp)
add_library(${name} STATIC ${src})
target_link_libraries(${name} PRIVATE kengine)

set(customFindPackageNames)
set(customLibraryNames recastnavigation:RecastNavigation)
putils_conan_download_and_link_packages_with_names(
	${name} PRIVATE
	"${customFindPackageNames}"
	"${customLibraryNames}"
	recastnavigation/cci.20200511
	glm/0.9.9.8
)