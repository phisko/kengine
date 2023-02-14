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

find_package(Qt5OpenGL 5.2)

set_package_properties(Qt5OpenGL PROPERTIES DESCRIPTION "C++ framework" URL http://qt-project.org)
set_package_properties(Qt5OpenGL PROPERTIES TYPE RECOMMENDED PURPOSE "Building the examples")

if(Qt5OpenGL_FOUND)
	SET(BUILD_EXAMPLES ON PARENT_SCOPE)
	ADD_SUBDIRECTORY(Basic)
	ADD_SUBDIRECTORY(Paging)
	ADD_SUBDIRECTORY(OpenGL)
	ADD_SUBDIRECTORY(SmoothLOD)
	ADD_SUBDIRECTORY(DecodeOnGPU)
	ADD_SUBDIRECTORY(Python)
else()
	SET(BUILD_EXAMPLES OFF PARENT_SCOPE)
endif()
