project(kengine)

if(KENGINE_SCRIPTING_LUA)
    system_link_private_libraries(kengine_scripting_lua)
endif()

if(KENGINE_SCRIPTING_PYTHON)
    system_link_private_libraries(kengine_scripting_python)
endif()