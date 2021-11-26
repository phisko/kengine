set(name kengine_recast)

putils_conan(recastnavigation/cci.20200511)

file(GLOB src *.cpp *.hpp)
add_library(${name} STATIC ${src})
target_link_libraries(${name} PRIVATE kengine CONAN_PKG::recastnavigation)