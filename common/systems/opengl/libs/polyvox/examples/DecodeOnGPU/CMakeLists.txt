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

PROJECT(DecodeOnGPUExample)

#Projects source files
SET(SRC_FILES
	main.cpp
	../common/PolyVoxExample.cpp
)

#Projects headers files	
SET(INC_FILES	
	../common/OpenGLWidget.h
	../common/OpenGLWidget.inl
	../common/PolyVoxExample.h
)

#"Sources" and "Headers" are the group names in Visual Studio.
#They may have other uses too...
SOURCE_GROUP("Sources" FILES ${SRC_FILES})
SOURCE_GROUP("Headers" FILES ${INC_FILES})

#Tell CMake the paths for OpenGL and for PolyVox (which is just relative to our current location)
INCLUDE_DIRECTORIES(${OPENGL_INCLUDE_DIR} ${PolyVoxHeaders_SOURCE_DIR} ../common)

#This will include the shader files inside the compiled binary
QT5_ADD_RESOURCES(COMMON_RESOURCES_RCC ../common/example.qrc)
QT5_ADD_RESOURCES(DECODE_RESOURCES_RCC decode.qrc)

# Put the resources in a seperate folder in Visual Studio
SOURCE_GROUP("Resource Files" FILES ../common/example.qrc ${COMMON_RESOURCES_RCC} decode.qrc ${DECODE_RESOURCES_RCC})

#Build
ADD_EXECUTABLE(DecodeOnGPUExample ${SRC_FILES} ${COMMON_RESOURCES_RCC} ${DECODE_RESOURCES_RCC})
IF(MSVC)
	SET_TARGET_PROPERTIES(DecodeOnGPUExample PROPERTIES COMPILE_FLAGS "/W4 /wd4127")
ENDIF(MSVC)
TARGET_LINK_LIBRARIES(DecodeOnGPUExample Qt5::OpenGL)
SET_PROPERTY(TARGET DecodeOnGPUExample PROPERTY FOLDER "Examples")

#Install - Only install the example in Windows
IF(WIN32)
	INSTALL(TARGETS DecodeOnGPUExample
		RUNTIME DESTINATION Examples/OpenGL/bin
		LIBRARY DESTINATION Examples/OpenGL/lib
		ARCHIVE DESTINATION Examples/OpenGL/lib
		COMPONENT example
	)
ENDIF(WIN32)
