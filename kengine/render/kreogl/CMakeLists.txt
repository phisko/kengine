project(kengine)

system_link_private_libraries(
		kengine_glm
		kengine_imgui
)

# Kreogl
set(KREOGL_PROFILING ${KENGINE_PROFILING})
add_subdirectory(libs/kreogl)
subdirectory_is_not_system(libs)
target_link_libraries(${system_name} PUBLIC kreogl)

# Shaders
file(GLOB shaders_src shaders/*.cpp shaders/*.hpp)
target_sources(${system_name} PRIVATE ${shaders_src})

# ImGui bindings
set(imgui_res_dir $<TARGET_PROPERTY:putils,BINARY_DIR>/imgui_res)
target_sources(${system_name} PRIVATE
		${imgui_res_dir}/bindings/imgui_impl_opengl3.cpp
		${imgui_res_dir}/bindings/imgui_impl_glfw.cpp)
target_include_directories(${system_name} PRIVATE ${imgui_res_dir})
putils_export_symbols(${system_name}
		EXPORT_MACRO_NAME IMGUI_IMPL_API
		EXPORT_FILE_NAME imgui_impl_api.h)