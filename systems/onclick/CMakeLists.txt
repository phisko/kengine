set(name kengine_onclick)

file(GLOB src *.cpp *.hpp)
add_library(${name} STATIC ${src})
target_link_libraries(${name} PRIVATE kengine)
