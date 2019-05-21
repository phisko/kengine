cmake_minimum_required(VERSION 3.0)
project(kengine)
set(CMAKE_CXX_STANDARD 17)

if (KENGINE_SFML)
    set(TGUI TRUE)
    set(PUTILS_BUILD_PSE TRUE)
endif ()

if(KENGINE_LUA)
    set(PUTILS_BUILD_LUA TRUE)
endif()

if(KENGINE_PYTHON)
    set(PUTILS_BUILD_PYTHON TRUE)
endif()

if (KENGINE_NO_SHADER_DEBUG)
    set(PUTILS_NO_SHADER_DEBUG TRUE)
endif()

set(PUTILS_BUILD_MEDIATOR TRUE)
add_subdirectory(putils)

file(GLOB src_files *.cpp common/systems/*.cpp)

if (UNIX)
  set(type SHARED)
elseif (WIN32)
    set(type STATIC)
endif ()

add_library(kengine ${type} ${src_files})
target_link_libraries(kengine PUBLIC putils)

if (KENGINE_SFML)
    add_subdirectory(common/systems/sfml)
    target_link_libraries(kengine PUBLIC kengine_sfml)
endif ()

if (KENGINE_OPENGL)
    add_subdirectory(common/systems/opengl)
    target_link_libraries(kengine PUBLIC kengine_opengl)
endif ()

if (KENGINE_ASSIMP)
    add_subdirectory(common/systems/assimp)
    target_link_libraries(kengine PUBLIC kengine_assimp)
endif()

if (KENGINE_POLYVOX)
    add_subdirectory(common/systems/polyvox)
    target_link_libraries(kengine PUBLIC kengine_polyvox)
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} PARENT_SCOPE)
target_include_directories(kengine PUBLIC . common)
