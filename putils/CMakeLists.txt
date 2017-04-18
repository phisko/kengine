set(new_includes
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/mediator
        ${CMAKE_CURRENT_LIST_DIR}/connection
        ${CMAKE_CURRENT_LIST_DIR}/meta
        ${CMAKE_CURRENT_LIST_DIR}/pse
        ${CMAKE_CURRENT_LIST_DIR}/pluginManager
        ${CMAKE_CURRENT_LIST_DIR}/http
        )
set(INCLUDE_DIRS ${INCLUDE_DIRS} ${new_includes} PARENT_SCOPE)
set(INCLUDE_DIRS ${INCLUDE_DIRS} ${new_includes})

include_directories(${INCLUDE_DIRS})

add_subdirectory(mediator)
add_subdirectory(connection)
add_subdirectory(meta)
add_subdirectory(pse)
add_subdirectory(pluginManager)
add_subdirectory(http)

foreach (dir ${dirlist})
    add_subdirectory(${dir})
endforeach()
