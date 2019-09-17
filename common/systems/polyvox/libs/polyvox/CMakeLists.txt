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

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.6)

PROJECT(PolyVox)

SET(POLYVOX_VERSION_MAJOR "0")
SET(POLYVOX_VERSION_MINOR "2")
SET(POLYVOX_VERSION_PATCH "1")
SET(POLYVOX_VERSION "${POLYVOX_VERSION_MAJOR}.${POLYVOX_VERSION_MINOR}.${POLYVOX_VERSION_PATCH}" CACHE STRING "PolyVox version")
MARK_AS_ADVANCED(FORCE POLYVOX_VERSION)

SET_PROPERTY(GLOBAL PROPERTY USE_FOLDERS ON)
include(FeatureSummary)

IF(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang") #Maybe "OR MINGW"
	ADD_DEFINITIONS(-std=c++0x) #Enable C++0x mode
ENDIF()

IF(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
	ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS)
ENDIF()

ADD_SUBDIRECTORY(include)

OPTION(ENABLE_EXAMPLES "Should the examples be built" ON)
IF(ENABLE_EXAMPLES)
	ADD_SUBDIRECTORY(examples)
ELSE()
	SET(BUILD_EXAMPLES OFF)
ENDIF()

INCLUDE(Packaging.cmake)

OPTION(ENABLE_TESTS "Should the tests be built" ON)
IF(ENABLE_TESTS)
	INCLUDE(CTest)
	MARK_AS_ADVANCED(FORCE BUILD_TESTING)
	ADD_SUBDIRECTORY(tests)
ELSE()
	SET(BUILD_TESTS OFF)
ENDIF()

ADD_SUBDIRECTORY(documentation)

ADD_SUBDIRECTORY(bindings)

add_feature_info("Examples" BUILD_EXAMPLES "Examples of PolyVox usage")
add_feature_info("Tests" BUILD_TESTS "Unit tests")
add_feature_info("Bindings" BUILD_BINDINGS "SWIG bindings")
add_feature_info("API docs" BUILD_DOCS "HTML documentation of the API")
add_feature_info("Manual" BUILD_MANUAL "HTML user's manual")

feature_summary(WHAT ALL)

# Option summary
MESSAGE(STATUS "")
MESSAGE(STATUS "Summary")
MESSAGE(STATUS "-------")
MESSAGE(STATUS "Build examples:      " ${BUILD_EXAMPLES})
MESSAGE(STATUS "Build tests:         " ${BUILD_TESTS})
MESSAGE(STATUS "Build bindings:      " ${BUILD_BINDINGS})
MESSAGE(STATUS "API Docs available:  " ${BUILD_DOCS})
MESSAGE(STATUS "Build manual:        " ${BUILD_MANUAL})
MESSAGE(STATUS "")
