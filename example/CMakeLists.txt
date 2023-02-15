cmake_minimum_required(VERSION 3.10)
project(kengine_example)

set(CMAKE_CXX_STANDARD 20)

# Alternatively, set KENGINE_ALL_SYSTEMS to compile everything
set(KENGINE_CORE ON CACHE BOOL "Enable kengine_core" FORCE)
set(KENGINE_MAIN_LOOP ON CACHE BOOL "Enable kengine_main_loop" FORCE)
set(KENGINE_LOG ON CACHE BOOL "Enable kengine_log" FORCE)
set(KENGINE_LOG_STDOUT ON CACHE BOOL "Enable kengine_log_stdout" FORCE)
set(KENGINE_SCRIPTING ON CACHE BOOL "Enable kengine_scripting" FORCE)
set(KENGINE_SCRIPTING_LUA ON CACHE BOOL "Enable kengine_scripting_lua" FORCE)
add_subdirectory(.. kengine)

add_executable(kengine_example main.cpp)
target_link_libraries(kengine_example PRIVATE kengine)

add_custom_command(TARGET kengine_example POST_BUILD COMMAND
		${CMAKE_COMMAND} -E copy_if_different
		${CMAKE_CURRENT_LIST_DIR}/unit.lua
		$<TARGET_FILE_DIR:kengine_example>/unit.lua)