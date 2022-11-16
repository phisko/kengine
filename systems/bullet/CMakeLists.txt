set(name kengine_bullet)

file(GLOB src *.cpp *.hpp)
add_library(${name} ${src})
target_link_libraries(${name} PRIVATE kengine_core)
putils_export_symbols(${name})

set(customFindPackageNames bullet3:Bullet)
set(customLibraryNames bullet3:Bullet)
if (NOT MSVC) # bullet3/3.21: Invalid ID: Shared libraries on Visual Studio not supported
	putils_conan_set_shared_options(options bullet3)
endif()
putils_conan_download_and_link_packages_with_names(
	${name} PRIVATE
	"${customFindPackageNames}"
	"${customLibraryNames}"
	bullet3/3.21
	${options}
)