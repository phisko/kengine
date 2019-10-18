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
	# Copy plugins.cfg
	if (NOT EXISTS ${dir}/plugins.cfg)
		file(COPY ${OGRE_CONFIG_DIR}/plugins.cfg DESTINATION ${dir})
	endif()

	# Copy resources.cfg
	if (NOT EXISTS ${dir}/resources.cfg)
		file(COPY ${OGRE_CONFIG_DIR}/resources.cfg DESTINATION ${dir})
	endif()

	# Copy DLLs according to configuration
	file(GLOB plugins ${OGRE_CONFIG_DIR}/*.dll ${OGRE_PLUGIN_DIR}/*.dll)

	if(CMAKE_BUILD_TYPE STREQUAL "Debug")
		if (EXISTS ${OGRE_CONFIG_DIR}/Debug)
			file(GLOB tmp ${OGRE_CONFIG_DIR}/Debug/*.dll)
			set(plugins ${plugins} ${tmp})
		endif()

		if (EXISTS ${OGRE_PLUGIN_DIR}/Debug)
			file(GLOB tmp ${OGRE_PLUGIN_DIR}/Debug/*.dll)
			set(plugins ${plugins} ${tmp})
		endif()
	else()
		if (EXISTS ${OGRE_CONFIG_DIR}/Release)
			file(GLOB tmp ${OGRE_CONFIG_DIR}/Release/*.dll)
			set(plugins ${plugins} ${tmp})
		endif()

		if (EXISTS ${OGRE_PLUGIN_DIR}/Release)
			file(GLOB tmp ${OGRE_PLUGIN_DIR}/Release/*.dll)
			set(plugins ${plugins} ${tmp})
		endif()
	endif()

	file(COPY ${plugins} DESTINATION ${dir})
endforeach(dir ${output_dirs})

# Copy Ogre Media dir
if (NOT EXISTS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Media)
	file(COPY ${OGRE_CONFIG_DIR}/../Media DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endif()

target_link_libraries(${name} PUBLIC ${OGRE_LIBRARIES})