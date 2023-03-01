project(kengine)

option(KENGINE_MAIN_LOOP "Build kengine_main_loop" OFF)
if(KENGINE_MAIN_LOOP)
        system_link_public_libraries(kengine_main_loop)
endif()
