project(kengine)

kengine_library_link_private_libraries(
		kengine_main_loop
)

find_package(SFML COMPONENTS system window graphics CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(ImGui-SFML CONFIG REQUIRED)
kengine_library_link_public_libraries(
	sfml-system sfml-window sfml-graphics
	ImGui-SFML::ImGui-SFML
)

find_package(GLEW REQUIRED)
kengine_library_link_private_libraries(
	GLEW::GLEW
)