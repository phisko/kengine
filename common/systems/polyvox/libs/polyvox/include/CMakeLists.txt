################################################################################
# The MIT License (MIT)
#
# Copyright (c) 2015 Matthew Williams and David Williams
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
################################################################################

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

PROJECT(PolyVoxHeaders)

#Projects headers files
SET(CORE_INC_FILES
	PolyVox/AmbientOcclusionCalculator.h
	PolyVox/AmbientOcclusionCalculator.inl
	PolyVox/Array.h
	PolyVox/AStarPathfinder.h
	PolyVox/AStarPathfinder.inl
	PolyVox/BaseVolume.h
	PolyVox/BaseVolume.inl
	PolyVox/BaseVolumeSampler.inl
	PolyVox/CubicSurfaceExtractor.h
	PolyVox/CubicSurfaceExtractor.inl
	PolyVox/DefaultIsQuadNeeded.h
	PolyVox/DefaultMarchingCubesController.h
	PolyVox/Density.h
	PolyVox/Exceptions.h
	PolyVox/FilePager.h
	PolyVox/Logging.h
	PolyVox/LowPassFilter.h
	PolyVox/LowPassFilter.inl
	PolyVox/MarchingCubesSurfaceExtractor.h
	PolyVox/MarchingCubesSurfaceExtractor.inl
	PolyVox/Material.h
	PolyVox/MaterialDensityPair.h
	PolyVox/Mesh.h
	PolyVox/Mesh.inl
	PolyVox/PagedVolume.h
	PolyVox/PagedVolume.inl
	PolyVox/PagedVolumeChunk.inl
	PolyVox/PagedVolumeSampler.inl
	PolyVox/Picking.h
	PolyVox/Picking.inl
	PolyVox/RawVolume.h
	PolyVox/RawVolume.inl
	PolyVox/RawVolumeSampler.inl
	PolyVox/Raycast.h
	PolyVox/Raycast.inl
	PolyVox/Region.h
	PolyVox/Region.inl
	PolyVox/Vector.h
	PolyVox/Vector.inl
	PolyVox/Vertex.h
	PolyVox/VolumeResampler.h
	PolyVox/VolumeResampler.inl
)

SET(IMPL_INC_FILES
	PolyVox/Impl/Assertions.h
	PolyVox/Impl/AStarPathfinderImpl.h
    PolyVox/Impl/Config.h
	PolyVox/Impl/ErrorHandling.h
	PolyVox/Impl/ExceptionsImpl.h
	PolyVox/Impl/Interpolation.h
	PolyVox/Impl/IteratorController.h
	PolyVox/Impl/IteratorController.inl
	PolyVox/Impl/LoggingImpl.h
	PolyVox/Impl/MarchingCubesTables.h
	PolyVox/Impl/PlatformDefinitions.h
	PolyVox/Impl/RandomUnitVectors.h
	PolyVox/Impl/RandomVectors.h
	PolyVox/Impl/Timer.h
	PolyVox/Impl/Utility.h
)

#NOTE: The following line should be uncommented when building shared libs.

#"Sources" and "Headers" are the group names in Visual Studio.
#They may have other uses too...
#SOURCE_GROUP("Source Files" FILES ${CORE_SRC_FILES})
SOURCE_GROUP("Header Files" FILES ${CORE_INC_FILES})

#SOURCE_GROUP("Source Files\\Impl" FILES ${IMPL_SRC_FILES})
SOURCE_GROUP("Header Files\\Impl" FILES ${IMPL_INC_FILES})

#Tell CMake the paths
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/include)

#Core

#Build
# Although we don't build anything for PolyVox, we still add this custom target (which
# doesn't do anything) so that we can browse the source code from within Visual Studio.
ADD_CUSTOM_TARGET(PolyVoxHeaders SOURCES ${CORE_INC_FILES})

#Install the core header files, including the ones in the Impl subfolder.
INSTALL(DIRECTORY PolyVox/ DESTINATION include/PolyVox COMPONENT development PATTERN "*.git*" EXCLUDE)

#Set up install paths e.g. for PolyVoxConfig.cmake
if(WIN32)
	set(CONFIG_FILE_DIR "CMake")
	set(PolyVoxCore_LIBRARY_INSTALL_DIRS "PolyVoxCore/lib")
	set(PolyVoxCore_INCLUDE_INSTALL_DIRS "PolyVoxCore/include")
	set(PolyVox_DOC_INSTALL_DIR "PolyVox/doc")
else(WIN32)
	set(CONFIG_FILE_DIR "share/PolyVox/cmake")
	set(PolyVoxCore_LIBRARY_INSTALL_DIRS "lib")
	set(PolyVoxCore_INCLUDE_INSTALL_DIRS "include/PolyVoxCore")
	set(PolyVox_DOC_INSTALL_DIR "share/doc/packages/polyvox")
endif(WIN32)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/PolyVoxConfig.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/PolyVoxConfig.cmake @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/PolyVoxConfig.cmake DESTINATION ${CONFIG_FILE_DIR} COMPONENT development)

FIND_PACKAGE(Doxygen)
set_package_properties(Doxygen PROPERTIES URL http://www.doxygen.org DESCRIPTION "API documentation generator" TYPE OPTIONAL PURPOSE "Building the API documentation")

#Documentation
if(DOXYGEN_FOUND)
	SET(BUILD_DOCS ON PARENT_SCOPE)
	#configure_file(${CMAKE_CURRENT_SOURCE_DIR}/polyvox.css ${CMAKE_CURRENT_BINARY_DIR}/polyvox.css)
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
	#This is just the default doc target which only runs Doxygen
	add_custom_target(doc
		COMMAND ${DOXYGEN_EXECUTABLE}
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
		COMMENT "Building documentation"
		SOURCES Doxyfile.in polyvox.qhcp.in Mainpage.dox
		VERBATIM
	)
	set_target_properties(doc PROPERTIES PROJECT_LABEL "API Reference") #Set label seen in IDE
	set_property(TARGET doc PROPERTY FOLDER "Documentation")
	
	#If we found qcollectiongenerator then do more processing
	if(QT_QCOLLECTIONGENERATOR_EXECUTABLE)
		configure_file(${CMAKE_CURRENT_SOURCE_DIR}/polyvox.qhcp.in ${CMAKE_CURRENT_BINARY_DIR}/doc/qthelp/polyvox.qhcp) #The QtHelp config file
		
		#We attach this command to the doc target so it will be run automatically
		add_custom_command(TARGET doc POST_BUILD
			COMMAND ${QT_QCOLLECTIONGENERATOR_EXECUTABLE} polyvox.qhcp -o polyvox.qhc
			WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doc/qthelp
			COMMENT "Compiling API documentation to Qt Help format"
		)
		
		install(FILES
			${CMAKE_CURRENT_BINARY_DIR}/doc/qthelp/polyvox.qhc
			${CMAKE_CURRENT_BINARY_DIR}/doc/qthelp/polyvox.qch
			DESTINATION ${PolyVox_DOC_INSTALL_DIR}/qthelp
			COMPONENT development
			OPTIONAL
		)
	endif()
else()
	SET(BUILD_DOCS OFF PARENT_SCOPE)
endif()
