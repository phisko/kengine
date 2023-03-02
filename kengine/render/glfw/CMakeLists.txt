project(kengine)

kengine_library_link_private_libraries(kengine_imgui)

find_package(glfw3 CONFIG REQUIRED)
kengine_library_link_public_libraries(glfw)

find_package(imgui CONFIG REQUIRED)
kengine_library_link_private_libraries(imgui::imgui)
