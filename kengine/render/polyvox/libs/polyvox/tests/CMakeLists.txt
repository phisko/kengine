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

find_package(Qt5Test 5.2)

set_package_properties(Qt5Test PROPERTIES DESCRIPTION "C++ framework" URL http://qt-project.org)
set_package_properties(Qt5Test PROPERTIES TYPE OPTIONAL PURPOSE "Building the tests")

# Creates a test from the inputs
#
# Also sets LATEST_TEST to point to the output executable of the test for easy
# ADD_TEST()ing

set(CMAKE_AUTOMOC TRUE)

MACRO(CREATE_TEST sourcefile executablename)
	UNSET(test_moc_SRCS) #clear out the MOCs from previous tests

	ADD_EXECUTABLE(${executablename} ${sourcefile} ${test_moc_SRCS})
	TARGET_LINK_LIBRARIES(${executablename} Qt5::Test)
	#HACK. This is needed since everything is built in the base dir in Windows. As of 2.8 we should change this.
	IF(WIN32)
		SET(LATEST_TEST ${EXECUTABLE_OUTPUT_PATH}/${executablename})
	ELSE(WIN32)
		SET(LATEST_TEST ${CMAKE_CURRENT_BINARY_DIR}/${executablename})
	ENDIF(WIN32)
	SET_PROPERTY(TARGET ${executablename} PROPERTY FOLDER "Tests")
	
	IF(${XML_TEST_OUTPUT})
		ADD_TEST(${executablename} ${LATEST_TEST} -xunitxml)
	ELSE()
		ADD_TEST(${executablename} ${LATEST_TEST})
	ENDIF()
ENDMACRO(CREATE_TEST)

INCLUDE_DIRECTORIES(${PolyVoxHeaders_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
#REMOVE_DEFINITIONS(-DQT_GUI_LIB) #Make sure the tests don't link to the QtGui

# Test Template. Copy and paste this template for consistant naming.
# # ClassName tests
# CREATE_TEST(TestClassName.cpp TestClassName)
# ADD_TEST(ClassNameFeature1Test ${LATEST_TEST} testFeature1)
# ADD_TEST(ClassNameFeature2Test ${LATEST_TEST} testFeature2)

# Python tests
IF(BUILD_BINDINGS)
	FIND_PACKAGE(PythonInterp 3)
	set_package_properties(PythonInterp PROPERTIES URL http://www.python.org DESCRIPTION "Python Interpereter" TYPE OPTIONAL PURPOSE "Running the Python tests")
	IF(PYTHONINTERP_FOUND)
		ADD_TEST(PythonSurfaceExtractorTest ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/TestSurfaceExtractor.py)
		ADD_TEST(PythonRaycastTest ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/TestRaycast.py)
	ENDIF()
ENDIF()

if(Qt5Test_FOUND)
	SET(BUILD_TESTS ON PARENT_SCOPE)
	
	# AmbientOcclusionGenerator tests
	CREATE_TEST(TestAmbientOcclusionGenerator.cpp TestAmbientOcclusionGenerator)
	
	# Array tests
	CREATE_TEST(TestArray.cpp TestArray)
	
	# AStarPathfinder tests
	CREATE_TEST(TestAStarPathfinder.cpp TestAStarPathfinder)
	
	CREATE_TEST(TestCubicSurfaceExtractor.cpp TestCubicSurfaceExtractor)
	
	# Low pass filter tests
	CREATE_TEST(TestLowPassFilter.cpp TestLowPassFilter)
	
	# Material tests
	CREATE_TEST(testmaterial.cpp testmaterial)
	
	# Raycast tests
	CREATE_TEST(TestRaycast.cpp TestRaycast)
	
	# Picking tests
	CREATE_TEST(TestPicking.cpp TestPicking)
	
	# Region tests
	CREATE_TEST(TestRegion.cpp TestRegion)
	
	CREATE_TEST(TestSurfaceExtractor.cpp TestSurfaceExtractor)
	
	#Vector tests
	CREATE_TEST(testvector.cpp testvector)
	
	# Volume tests
	CREATE_TEST(testvolume.cpp testvolume)
	
	# Volume subclass tests
	CREATE_TEST(TestVolumeSubclass.cpp TestVolumeSubclass)
else()
	SET(BUILD_TESTS OFF PARENT_SCOPE)
endif()