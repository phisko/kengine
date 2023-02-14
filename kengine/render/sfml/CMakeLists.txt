project(kengine)

system_link_private_libraries(
		kengine_main_loop
)

# SFML
set(customFindPackageNames sfml:SFML)
set(customLibraryNames sfml:SFML)
putils_conan_set_shared_options(options sfml)
list(APPEND options sfml:audio=False) # doesn't compile in C++20, still uses auto_ptr
putils_conan_download_and_link_packages_with_names(
	${system_name} PUBLIC
	"${customFindPackageNames}"
	"${customLibraryNames}"
	sfml/2.5.1
	${options}
)

# doesn't compile on mingw with the following error
# cpuid.h:334:1: error: 'void __cpuidex(int*, int, int)' redeclared inline without 'gnu_inline' attribute

# ImGui SFML
file(GLOB src
		libs/imgui-sfml/*.cpp libs/imgui-sfml/*.hpp)
subdirectory_is_not_system(libs)
target_sources(${system_name} PRIVATE ${src})
target_include_directories(${system_name} PUBLIC libs)
target_compile_definitions(${system_name} PUBLIC IMGUI_SFML_SHARED_LIB) # See imgui-SFML_export.h
target_compile_definitions(${system_name} PRIVATE IMGUI_SFML_EXPORTS) # See imgui-SFML_export.h