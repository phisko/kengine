set(name kengine_bullet)

putils_conan(bullet3/2.88@bincrafters/stable)

file(GLOB src *.cpp *.hpp)
add_library(${name} STATIC ${src})
target_link_libraries(${name} PRIVATE kengine CONAN_PKG::bullet3)