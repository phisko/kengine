cmake_minimum_required(VERSION 3.0)
project(kengine)

add_subdirectory(putils)

set(new_includes
        ${CMAKE_CURRENT_LIST_DIR}/mediator
        ${CMAKE_CURRENT_LIST_DIR}
        )
set(INCLUDE_DIRS ${INCLUDE_DIRS} ${new_includes} PARENT_SCOPE)
include_directories(${INCLUDE_DIRS})

file(GLOB src_files
        *.cpp
        *.hpp
        )

add_library(kengine SHARED ${src_files})
target_link_libraries(kengine mediator)
