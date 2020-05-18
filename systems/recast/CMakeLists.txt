set(name kengine_recast)

conan_add_remote(NAME arsen-deps URL https://api.bintray.com/conan/arsen-studio/arsen-deps )
putils_conan(recast/1.5.1@arsen-studio/stable)

file(GLOB src *.cpp *.hpp)
add_library(${name} STATIC ${src})
target_link_libraries(${name} PRIVATE kengine CONAN_PKG::recast)