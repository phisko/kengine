cmake_minimum_required(VERSION 3.0)
project(kengine)
set(CMAKE_CXX_STANDARD 17)
if(WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /DNOMINMAX")
endif()

set(PUTILS_BUILD_PSE ${KENGINE_SFML})
set(PUTILS_BUILD_LUA ${KENGINE_LUA})
set(PUTILS_BUILD_PYTHON ${KENGINE_PYTHON})
set(PUTILS_NO_SHADER_DEBUG ${KENGINE_NO_SHADER_DEBUG})
add_subdirectory(putils)

if (KENGINE_LUA)
    add_compile_definitions(KENGINE_LUA)
endif()

if (KENGINE_PYTHON)
    add_compile_definitions(KENGINE_PYTHON)
endif()

file(GLOB src_files
    *.cpp *.hpp
    components/data/*.cpp components/data/*.hpp
    components/functions/*.cpp components/functions/*.hpp
    components/meta/*.cpp components/meta/*.hpp
    systems/*.cpp systems/*.hpp
    helpers/*.cpp helpers/*.hpp)

add_library(kengine STATIC ${src_files})
target_link_libraries(kengine PUBLIC putils)

if (KENGINE_SFML)
    add_subdirectory(systems/sfml)
    target_link_libraries(kengine PUBLIC kengine_sfml)
endif ()

if (KENGINE_OPENGL)
    set(BUILD_UTILS FALSE)
    set(GLEW_PATH systems/opengl/libs/glew)
    add_subdirectory(${GLEW_PATH}/build/cmake)
    target_link_libraries(kengine PUBLIC glew)
    target_include_directories(kengine PUBLIC ${GLEW_PATH}/include)

    putils_conan(glm/0.9.9.5@g-truc/stable)
    target_link_libraries(kengine PUBLIC CONAN_PKG::glm)

    add_subdirectory(systems/opengl)
    target_link_libraries(kengine PUBLIC kengine_opengl)

    if (NOT KENGINE_OPENGL_NO_DEFAULT_SHADERS)
        add_subdirectory(systems/opengl_sprites)
        target_link_libraries(kengine PUBLIC kengine_opengl_sprites)
    endif()
endif ()

if (KENGINE_ASSIMP)
    add_subdirectory(systems/assimp)
    target_link_libraries(kengine PUBLIC kengine_assimp)
endif()

if (KENGINE_POLYVOX)
    add_subdirectory(systems/polyvox)
    target_link_libraries(kengine PUBLIC kengine_polyvox)
endif()

if (KENGINE_BULLET)
    add_subdirectory(systems/bullet)
    target_link_libraries(kengine PUBLIC kengine_bullet)
endif()

if (KENGINE_OGRE)
    add_subdirectory(systems/ogre)
    target_link_libraries(kengine PUBLIC kengine_ogre)
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} PARENT_SCOPE)
target_include_directories(kengine PUBLIC . components)
