#
# SFML System
#

set(name sfml)

file(GLOB src
        *.cpp *.hpp)

add_library(${name}
        SHARED MODULE
        ${src}
        )
target_link_libraries(${name} kengine pse pluginManager)
target_include_directories(${name} PRIVATE ${CMAKE_CURRENT_LIST_DIR})
