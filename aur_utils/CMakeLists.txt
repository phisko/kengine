cmake_minimum_required(VERSION 3.0)

set(project_name "example")
set(engine_dir "engine")
# helper_placeholder

project(${project_name})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin/")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin/")

include_directories(include)

# Set this flag to build and link with default sfml engine
set(PUTILS_BUILD_PSE TRUE)

# Set this to build and link with default Ogre3D engine
#set(PUTILS_BUILD_POGRE TRUE)

add_subdirectory(${engine_dir})

file(GLOB SOURCE_FILES
        src/*.cpp
        include/*.hpp)

add_executable(${project_name} ${SOURCE_FILES})
target_link_libraries(${project_name} ${OpenVC_LIBS} ${Boost_LIBS} kengine)

file(COPY "resources" DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

# Keep this line and setup a sf-config.json file accordingly to override default settings
file(COPY "sf-config.json" DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
