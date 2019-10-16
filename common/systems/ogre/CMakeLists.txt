set(name kengine_ogre)

set(IMGUI_DIR ../../../putils/imgui)
set(IMGUI_FREETYPE_DIR ${IMGUI_DIR}/misc/freetype)

file(GLOB src
	*.cpp *.hpp
	${IMGUI_FREETYPE_DIR}/imgui_freetype.cpp
)

add_library(${name} STATIC ${src})

# For now, users have to install Ogre themselves
# Hoping for someone to make a Conan repo

find_package(OGRE REQUIRED)
putils_get_runtime_output_directories(output_dirs)
foreach(dir ${output_dirs})
	file(COPY ${OGRE_CONFIG_DIR}/plugins.cfg DESTINATION ${dir})

	file(GLOB plugins ${OGRE_CONFIG_DIR}/*.dll ${OGRE_PLUGIN_DIR}/*.dll)
	file(COPY ${plugins} DESTINATION ${dir})
endforeach(dir ${output_dirs})

# putils_conan(
# 	sdl2/2.0.10@bincrafters/stable
# )
# 
# conan_add_remote(NAME anotherfoxguy URL https://api.bintray.com/conan/anotherfoxguy/ror-dependencies)
# conan_cmake_run(
# 	REQUIRES
# 		OGRE/1.11.6-with-patches@anotherfoxguy/stable
# 		# OGRE/1.12.2@anotherfoxguy/testing
# 	BASIC_SETUP CMAKE_TARGETS
# 	BUILD missing
# 	IMPORTS
# 		"bin, *.dll -> ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
# 		"bin, *.dll -> ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}"
# 		"bin, *.dll -> ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}"
# 		"bin, *.dll -> ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO}"
# 		"bin, *.dll -> ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL}"
# 
# 		"lib, *.dll -> ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
# 		"lib, *.dll -> ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}"
# 		"lib, *.dll -> ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}"
# 		"lib, *.dll -> ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO}"
# 		"lib, *.dll -> ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL}"
# 
# 		"bin, plugins*.cfg -> ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
# 		"bin, plugins*.cfg -> ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}"
# 		"bin, plugins*.cfg -> ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE}"
# 		"bin, plugins*.cfg -> ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO}"
# 		"bin, plugins*.cfg -> ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL}"
# )

target_link_libraries(${name} PUBLIC ${OGRE_LIBRARIES})

putils_conan(freetype/2.10.0@bincrafters/stable)
target_link_libraries(${name} PRIVATE kengine CONAN_PKG::freetype)

target_include_directories(${name} PRIVATE ${IMGUI_FREETYPE_DIR})