#
# SFML System
#

set(name kengine_polyvox)

file(GLOB src *.cpp)

if (UNIX)
    set(type SHARED)
elseif (WIN32)
    set(type STATIC)
endif ()

add_library(${name} ${type} ${src})
target_link_libraries(${name} PUBLIC kengine)
target_include_directories(${name} PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
	${CMAKE_CURRENT_LIST_DIR}/libs/polyvox/include
)
