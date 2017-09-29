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
target_link_libraries(${name} kengine pse pluginManager lua)
target_include_directories(${name} PRIVATE ${CMAKE_CURRENT_LIST_DIR})

file(COPY sf-config.json DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
