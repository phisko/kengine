set(name kengine_ogre)

set(IMGUI_DIR ../../../putils/imgui)
set(IMGUI_FREETYPE_DIR ${IMGUI_DIR}/misc/freetype)

file(GLOB src
	*.cpp *.hpp
	${IMGUI_FREETYPE_DIR}/imgui_freetype.cpp
)

add_library(${name} STATIC ${src})

putils_conan(freetype/2.10.0@bincrafters/stable)
target_link_libraries(${name} PRIVATE kengine CONAN_PKG::freetype)
target_include_directories(${name} PRIVATE ${IMGUI_FREETYPE_DIR})

# For now, users have to install Ogre themselves
# Hoping for someone to make a Conan repo

find_package(OGRE REQUIRED)
putils_get_runtime_output_directories(output_dirs)
foreach(dir ${output_dirs})
	file(COPY ${OGRE_CONFIG_DIR}/plugins.cfg DESTINATION ${dir})

	file(GLOB plugins ${OGRE_CONFIG_DIR}/*.dll ${OGRE_PLUGIN_DIR}/*.dll)
	file(COPY ${plugins} DESTINATION ${dir})
endforeach(dir ${output_dirs})
target_link_libraries(${name} PUBLIC ${OGRE_LIBRARIES})