set(name kengine_imgui_entity_editor)

file(GLOB src *.cpp *.hpp)
add_library(${name} ${src})
target_link_libraries(${name} PRIVATE kengine_core)
putils_export_symbols(${name})