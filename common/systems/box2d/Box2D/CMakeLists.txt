cmake_minimum_required(VERSION 2.8)

# Specify default build type if none provided
if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build (Debug or Release)" FORCE)
endif()

project(box2d)

set(BOX2D_VERSION 2.3.2)

# Windows: Choose to link runtime libraries statically or dynamically
if(WIN32)
	option(BOX2D_STATIC_STD_LIBS "Use statically linked standard/runtime libraries? This option must match the one used for SFML." OFF)

	# Determine whether we're dealing with a TDM compiler or not
	if(CMAKE_COMPILER_IS_GNUCXX)
		execute_process(COMMAND "${CMAKE_CXX_COMPILER}" "--version" OUTPUT_VARIABLE GCC_COMPILER_VERSION)
		string(REGEX MATCHALL ".*(tdm[64]*-[1-9]).*" COMPILER_GCC_TDM "${GCC_COMPILER_VERSION}")
	endif()

	# Modify compiler flags
	if(BOX2D_STATIC_STD_LIBS)
		if(MSVC)
			foreach(flag CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE)
				if(${flag} MATCHES "/MD")
					string(REGEX REPLACE "/MD" "/MT" ${flag} "${${flag}}")
				endif()
			endforeach()
		elseif(CMAKE_COMPILER_IS_GNUCXX AND NOT COMPILER_GCC_TDM)
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static")
		endif()
	elseif(COMPILER_GCC_TDM)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -shared-libgcc -shared-libstdc++")
	endif()
endif()

# The Box2D library.
add_subdirectory(src)

install(FILES README.md LICENSE DESTINATION ${CMAKE_INSTALL_PREFIX})
