#
# Plugin Manager
#

file(GLOB src
		*.cpp *.hpp)

set(new_includes
        ${CMAKE_CURRENT_LIST_DIR}
        )
set(INCLUDE_DIRS ${INCLUDE_DIRS} ${new_includes} PARENT_SCOPE)
include_directories(${INCLUDE_DIRS})

add_library(pluginManager STATIC ${src})

set(LIBRARIES
	${LIBRARIES}
	pluginManager
	PARENT_SCOPE
)
