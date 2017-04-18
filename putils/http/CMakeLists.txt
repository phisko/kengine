cmake_minimum_required(VERSION 3.6)
project(kia)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
if (UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W -Wall -Wextra")
endif ()

set(INCLUDE_DIRS ${INCLUDE_DIRS} . PARENT_SCOPE)

include_directories(${INCLUDE_DIRS})
include_directories(src)
include_directories(include)
link_directories(lib)

message(ERROR "BITEUH")

file(GLOB SOURCE_FILES
        *.cpp
        *.hpp
        modules/*.cpp
        modules/*.hpp
        )

add_library(http SHARED ${SOURCE_FILES})
target_link_libraries(http mediator ssl crypto)

if (UNIX)
    target_link_libraries(http pthread)
endif ()
