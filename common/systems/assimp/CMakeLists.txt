set(name kengine_assimp)

putils_conan(Assimp/4.1.0@jacmoe/stable)

file(GLOB src *.cpp *.hpp)
add_library(${name} STATIC ${src})
target_link_libraries(${name} PRIVATE kengine CONAN_PKG::Assimp)