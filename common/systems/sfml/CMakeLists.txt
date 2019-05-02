#
# SFML System
#

set(name kengine_sfml)

file(GLOB src
        *.cpp *.hpp)

if (UNIX)
    set(type SHARED)
elseif (WIN32)
    set(type STATIC)
endif ()

add_library(${name} ${type} ${src})
target_link_libraries(${name} kengine pse pluginManager lua)
target_include_directories(${name} PRIVATE ${CMAKE_CURRENT_LIST_DIR})

file(COPY sf-config.json DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
