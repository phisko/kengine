project(kengine)

find_package(imgui CONFIG REQUIRED)
kengine_library_link_public_libraries(imgui::imgui)
