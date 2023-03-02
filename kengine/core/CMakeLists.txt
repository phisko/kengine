project(kengine)

if(KENGINE_TESTS)
	target_link_libraries(${kengine_library_tests_name} PRIVATE kengine_core_log)
endif()
