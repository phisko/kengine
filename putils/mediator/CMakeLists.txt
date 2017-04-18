set(new_includes
        ${CMAKE_CURRENT_LIST_DIR}
        )
set(INCLUDE_DIRS ${INCLUDE_DIRS} ${new_includes} PARENT_SCOPE)
include_directories(${INCLUDE_DIRS})

file(GLOB src *.cpp *.hpp)

add_library(mediator SHARED ${src})
if (UNIX)
    target_link_libraries(mediator pthread)
endif()
