set(name kengine_assimp)

putils_conan(assimp/5.1.0) # doesn't compile on mingw in debug with the following error:
# as: CMakeFiles\assimp.dir\AssetLib\IFC\IFCReaderGen1_2x3.cpp.obj: too many sections (65896)
# Assembler messages:
# Fatal error: can't write 180 bytes to section .text of CMakeFiles\assimp.dir\AssetLib\IFC\IFCReaderGen1_2x3.cpp.obj: 'file too big'


file(GLOB src *.cpp *.hpp)
add_library(${name} STATIC ${src})
target_link_libraries(${name} PRIVATE kengine CONAN_PKG::assimp)