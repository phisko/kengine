#
# Box2D System
#

set(name b2d)

file(GLOB src
        *.cpp *.hpp)

set(BOX2D_BUILD_SHARED TRUE)
add_subdirectory(Box2D)

add_library(${name}
        SHARED MODULE
        ${src}
        )
target_link_libraries(${name} kengine box2d pluginManager)
target_include_directories(${name} PRIVATE ${CMAKE_CURRENT_LIST_DIR} Box2D/include)
