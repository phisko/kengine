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

find_program(SPHINXBUILD_EXECUTABLE sphinx-build DOC "The location of the sphinx-build executable")

#if(SPHINXBUILD_EXECUTABLE AND QT_QCOLLECTIONGENERATOR_EXECUTABLE)
if(SPHINXBUILD_EXECUTABLE)
	message(STATUS "Found `sphinx-build` at ${SPHINXBUILD_EXECUTABLE}")
	set(BUILD_MANUAL ON PARENT_SCOPE)
	
	configure_file(${CMAKE_CURRENT_SOURCE_DIR}/conf.in.py ${CMAKE_CURRENT_BINARY_DIR}/conf.py @ONLY)
	#Generates the HTML docs and the Qt help file which can be opened with "assistant -collectionFile thermite.qhc"
	#add_custom_target(manual ${SPHINXBUILD_EXECUTABLE} -b qthelp ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} COMMAND ${QT_QCOLLECTIONGENERATOR_EXECUTABLE} polyvox.qhcp -o polyvox.qhc)
	add_custom_target(manual 
		${SPHINXBUILD_EXECUTABLE} -b html
		-c ${CMAKE_CURRENT_BINARY_DIR} #Load the conf.py from the binary dir
		${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
		COMMENT "Building PolyVox manual"
	)
	add_dependencies(manual doc)
	set_target_properties(manual PROPERTIES PROJECT_LABEL "Manual") #Set label seen in IDE
	SET_PROPERTY(TARGET manual PROPERTY FOLDER "Documentation")
else()
	if(NOT SPHINXBUILD_EXECUTABLE)
		message(STATUS "`sphinx-build` was not found. Try setting SPHINXBUILD_EXECUTABLE to its location.")
	endif()
	if(NOT QT_QCOLLECTIONGENERATOR_EXECUTABLE)
		message(STATUS "`qhelpgenerator` was not found. Try setting QT_QCOLLECTIONGENERATOR_EXECUTABLE to its location.")
	endif()
	set(BUILD_MANUAL OFF PARENT_SCOPE)
endif()
