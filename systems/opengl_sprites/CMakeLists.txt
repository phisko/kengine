set(name kengine_opengl_sprites)

file(GLOB src *.cpp *.hpp)
add_library(${name} STATIC ${src})
target_link_libraries(${name} PUBLIC kengine)
target_include_directories(${name} PRIVATE ${CMAKE_CURRENT_LIST_DIR})
