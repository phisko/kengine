cmake_minimum_required(VERSION 3.10)
project(kengine_example)

set(CMAKE_CXX_STANDARD 20)

set(KENGINE_LOG_STDOUT ON)
set(KENGINE_LUA ON)
add_subdirectory(.. kengine)

add_executable(kengine_example main.cpp)
target_link_libraries(kengine_example PRIVATE kengine)

add_custom_command(TARGET kengine_example POST_BUILD COMMAND
		${CMAKE_COMMAND} -E copy_if_different
		${CMAKE_CURRENT_LIST_DIR}/unit.lua
		$<TARGET_FILE_DIR:kengine_example>/unit.lua)