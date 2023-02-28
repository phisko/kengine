project(kengine)

if(KENGINE_TESTS)
	target_link_libraries(${system_tests_name} PRIVATE kengine_core_log)
endif()
