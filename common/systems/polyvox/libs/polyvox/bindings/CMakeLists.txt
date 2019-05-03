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

option(ENABLE_BINDINGS "Build bindings" OFF) #Off by default
if(ENABLE_BINDINGS)
	find_package(SWIG)
	mark_as_advanced(SWIG_DIR SWIG_VERSION)
	find_package(PythonLibs 3)
	if(CMAKE_VERSION VERSION_LESS "2.8.6")
		set_package_info(SWIG "Bindings generator" http://www.swig.org)
		set_package_info(PythonLibs "Programming language" http://www.python.org)
	else()
		set_package_properties(SWIG PROPERTIES DESCRIPTION "Bindings generator" URL http://www.swig.org)
		set_package_properties(PythonLibs PROPERTIES DESCRIPTION "Programming language" URL http://www.python.org)
	endif()
	if(SWIG_FOUND)
		set(BUILD_BINDINGS ON CACHE BOOL "Will the bindings be built" FORCE)
		include(${SWIG_USE_FILE})
		
		set(CMAKE_SWIG_FLAGS "")
		set_source_files_properties(PolyVoxCore.i PROPERTIES CPLUSPLUS ON)
		
		include_directories(${PolyVoxHeaders_SOURCE_DIR} ${PolyVoxHeaders_SOURCE_DIR}/PolyVox)
		if(PYTHONLIBS_FOUND)
			include_directories(${PYTHON_INCLUDE_PATH})
			link_directories(${PolyVoxCore_BINARY_DIR})
			
			#set_source_files_properties(PolyVoxCore.i PROPERTIES SWIG_FLAGS "-builtin")
			set(SWIG_MODULE_PolyVoxCorePython_EXTRA_FLAGS "-py3")
			swig_add_module(PolyVoxCorePython python PolyVoxCore.i)
			swig_link_libraries(PolyVoxCorePython ${PYTHON_LIBRARIES})
			set_target_properties(${SWIG_MODULE_PolyVoxCorePython_REAL_NAME} PROPERTIES OUTPUT_NAME _PolyVoxCore)
			#set_target_properties(${SWIG_MODULE_PolyVoxCore_REAL_NAME} PROPERTIES SUFFIX ".pyd")
			SET_PROPERTY(TARGET ${SWIG_MODULE_PolyVoxCorePython_REAL_NAME} PROPERTY FOLDER "Bindings")
		endif()
		
		set(SWIG_MODULE_PolyVoxCoreCSharp_EXTRA_FLAGS "-dllimport;PolyVoxCoreCSharp") #This _should_ be inside UseSWIG.cmake - http://www.cmake.org/Bug/view.php?id=13814
		swig_add_module(PolyVoxCoreCSharp csharp PolyVoxCore.i)
		swig_link_libraries(PolyVoxCoreCSharp)
		SET_PROPERTY(TARGET ${SWIG_MODULE_PolyVoxCoreCSharp_REAL_NAME} PROPERTY FOLDER "Bindings")
	else()
		set(BUILD_BINDINGS OFF CACHE BOOL "Will the bindings be built" FORCE)
	endif()
else()
	set(BUILD_BINDINGS OFF CACHE BOOL "Will the bindings be built" FORCE)
endif()
mark_as_advanced(FORCE BUILD_BINDINGS)
