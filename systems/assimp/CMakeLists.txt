set(name kengine_assimp)

putils_conan(assimp/5.1.0)

file(GLOB src *.cpp *.hpp)
add_library(${name} STATIC ${src})
target_link_libraries(${name} PRIVATE kengine CONAN_PKG::assimp)