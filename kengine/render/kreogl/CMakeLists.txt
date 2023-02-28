project(kengine)

system_link_private_libraries(
		kengine_glm
		kengine_imgui
)

# Kreogl
set(KREOGL_PROFILING ${KENGINE_PROFILING})
add_subdirectory(libs/kreogl)
subdirectory_is_not_system(libs)
system_link_public_libraries(kreogl)

# Shaders
file(GLOB shaders_src shaders/*.cpp shaders/*.hpp)
target_sources(${system_name} PRIVATE ${shaders_src})