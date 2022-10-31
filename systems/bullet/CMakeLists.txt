set(name kengine_bullet)

file(GLOB src *.cpp *.hpp)
add_library(${name} STATIC ${src})
target_link_libraries(${name} PRIVATE kengine)

set(customFindPackageNames bullet3:Bullet)
set(customLibraryNames bullet3:Bullet)
putils_conan_download_and_link_packages_with_names(
	${name} PRIVATE
	"${customFindPackageNames}"
	"${customLibraryNames}"
	bullet3/3.21
	glm/0.9.9.8
)