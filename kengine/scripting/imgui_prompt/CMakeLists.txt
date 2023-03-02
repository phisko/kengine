project(kengine)

kengine_library_link_private_libraries(kengine_imgui)

if(KENGINE_SCRIPTING_LUA)
    kengine_library_link_private_libraries(kengine_scripting_lua)
endif()

if(KENGINE_SCRIPTING_PYTHON)
    kengine_library_link_private_libraries(kengine_scripting_python)
endif()