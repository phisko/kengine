set(name kengine_imgui_tool)

file(GLOB src *.cpp *.hpp)
add_library(${name} STATIC ${src})
target_link_libraries(${name} PRIVATE kengine)
