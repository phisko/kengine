set(name ogre)

file(GLOB src
        *.cpp *.hpp)

add_library(${name}
        SHARED MODULE
        ${src}
        )
target_include_directories(${name} PRIVATE .)
target_link_libraries(${name} kengine pogre lua)
