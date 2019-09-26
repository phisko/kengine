set(name kengine_imgui_overlay)

set(IMGUI_DIR ../../../putils/imgui)
file(GLOB src
	*.cpp *.hpp
	${IMGUI_DIR}/examples/imgui_impl_glfw.cpp
	${IMGUI_DIR}/examples/imgui_impl_opengl3.cpp
)

add_library(${name} STATIC ${src})

target_compile_definitions(${name} PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLEW)
target_include_directories(${name} PUBLIC
	${CMAKE_CURRENT_LIST_DIR}
	${CMAKE_CURRENT_LIST_DIR}/libs/glew/include
)

putils_conan(glfw/3.3@bincrafters/stable)
target_link_libraries(${name} PUBLIC kengine CONAN_PKG::glfw)

configure_file(koala.ico ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/resources/koala.ico COPYONLY)
configure_file(koala.ico ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}/resources/koala.ico COPYONLY)
configure_file(koala.ico ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}/resources/koala.ico COPYONLY)
configure_file(koala.ico ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO}/resources/koala.ico COPYONLY)
configure_file(koala.ico ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL}/resources/koala.ico COPYONLY)
