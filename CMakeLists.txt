cmake_minimum_required(VERSION 3.0)
project(kengine)

# Options

if (KENGINE_ALL_SYSTEMS)
    set(KENGINE_ASSIMP TRUE)
    set(KENGINE_BULLET TRUE)
    set(KENGINE_COLLISION TRUE)
    set(KENGINE_GLFW TRUE)
    set(KENGINE_IMGUI_ADJUSTABLE TRUE)
    set(KENGINE_IMGUI_ENGINE_STATS TRUE)
    set(KENGINE_IMGUI_ENTITY_EDITOR TRUE)
    set(KENGINE_IMGUI_ENTITY_SELECTOR TRUE)
    set(KENGINE_IMGUI_PROMPT TRUE)
    set(KENGINE_IMGUI_TOOL TRUE)
    set(KENGINE_INPUT TRUE)
    set(KENGINE_KINEMATIC TRUE)
    set(KENGINE_LOG_IMGUI TRUE)
    set(KENGINE_LOG_STDOUT TRUE)
    set(KENGINE_LOG_VISUAL_STUDIO TRUE)
    set(KENGINE_LUA TRUE)
    set(KENGINE_MODEL_CREATOR TRUE)
    set(KENGINE_ONCLICK TRUE)
    set(KENGINE_OPENGL TRUE)
    set(KENGINE_OPENGL_SPRITES TRUE)
    set(KENGINE_POLYVOX TRUE)
    set(KENGINE_PYTHON TRUE)
    set(KENGINE_RECAST TRUE)
    set(KENGINE_SFML TRUE)
endif()

set(CMAKE_CXX_STANDARD 20)
if (WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /DNOMINMAX")
endif()

set(PUTILS_BUILD_PSE ${KENGINE_SFML})
set(PUTILS_BUILD_LUA ${KENGINE_LUA})
set(PUTILS_BUILD_PYTHON ${KENGINE_PYTHON})
set(PUTILS_NO_SHADER_DEBUG ${KENGINE_NO_SHADER_DEBUG})
set(PUTILS_BUILD_OPENGL ${KENGINE_OPENGL})
add_subdirectory(putils)

# Add C++ macros

putils_src_files(src_files
	.
	impl
	components/data
	components/functions
	components/meta
	helpers
	helpers/meta
)

add_library(kengine STATIC ${src_files})
target_link_libraries(kengine PUBLIC putils)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} PARENT_SCOPE)
target_include_directories(kengine PUBLIC . components)

if (WIN32)
    if (NOT KENGINE_NDEBUG)
        target_link_libraries(kengine PUBLIC Dbghelp)
    endif()
endif()

# Add systems

macro(ADD_SYSTEM)
    if (${ARGV0})
        target_compile_definitions(kengine PUBLIC ${ARGV0})
        add_subdirectory(${ARGV1})
        target_link_libraries(kengine PUBLIC ${ARGV2})
    endif()
endmacro()

ADD_SYSTEM(KENGINE_ASSIMP systems/assimp kengine_assimp)
ADD_SYSTEM(KENGINE_BULLET systems/bullet kengine_bullet)
ADD_SYSTEM(KENGINE_COLLISION systems/collision kengine_collision)
ADD_SYSTEM(KENGINE_GLFW systems/glfw kengine_glfw)
ADD_SYSTEM(KENGINE_IMGUI_ADJUSTABLE systems/imgui_adjustable kengine_imgui_adjustable)
ADD_SYSTEM(KENGINE_IMGUI_ENGINE_STATS systems/imgui_engine_stats kengine_imgui_engine_stats)
ADD_SYSTEM(KENGINE_IMGUI_ENTITY_EDITOR systems/imgui_entity_editor kengine_imgui_entity_editor)
ADD_SYSTEM(KENGINE_IMGUI_ENTITY_SELECTOR systems/imgui_entity_selector kengine_imgui_entity_selector)
ADD_SYSTEM(KENGINE_IMGUI_PROMPT systems/imgui_prompt kengine_imgui_prompt)
ADD_SYSTEM(KENGINE_IMGUI_TOOL systems/imgui_tool kengine_imgui_tool)
ADD_SYSTEM(KENGINE_INPUT systems/input kengine_input)
ADD_SYSTEM(KENGINE_KINEMATIC systems/kinematic kengine_kinematic)
ADD_SYSTEM(KENGINE_LOG_IMGUI systems/log_imgui kengine_log_imgui)
ADD_SYSTEM(KENGINE_LOG_STDOUT systems/log_stdout kengine_log_stdout)
ADD_SYSTEM(KENGINE_LOG_VISUAL_STUDIO systems/log_visual_studio kengine_log_visual_studio)
ADD_SYSTEM(KENGINE_LUA systems/lua kengine_lua)
ADD_SYSTEM(KENGINE_MODEL_CREATOR systems/model_creator kengine_model_creator)
ADD_SYSTEM(KENGINE_ONCLICK systems/onclick kengine_onclick)
ADD_SYSTEM(KENGINE_OPENGL systems/opengl kengine_opengl)
ADD_SYSTEM(KENGINE_OPENGL_SPRITES systems/opengl_sprites kengine_opengl_sprites)
ADD_SYSTEM(KENGINE_POLYVOX systems/polyvox kengine_polyvox)
ADD_SYSTEM(KENGINE_PYTHON systems/python kengine_python)
ADD_SYSTEM(KENGINE_RECAST systems/recast kengine_recast)
ADD_SYSTEM(KENGINE_SFML systems/sfml kengine_sfml)
