project(kengine)

kengine_library_link_private_libraries(
		kengine_glm
		kengine_imgui
)

# Kreogl
set(KREOGL_PROFILING ${KENGINE_PROFILING})
add_subdirectory(libs/kreogl)
subdirectory_is_not_kengine_library(libs)
kengine_library_link_public_libraries(kreogl)

# Shaders
file(GLOB shaders_src shaders/*.cpp shaders/*.hpp)
target_sources(${kengine_library_name} PRIVATE ${shaders_src})