#pragma once

#ifndef KENGINE_ASYNC_TASK_STRING_MAX_LENGTH
#define KENGINE_ASYNC_TASK_STRING_MAX_LENGTH 64
#endif

// stl
#include <chrono>

// putils
#include "putils/string.hpp"

namespace kengine::data {
	//! putils reflect all
	//! used_types: [refltype::string]
	struct async_task {
		static constexpr char string_name[] = "async_task_string";
		using string = putils::string<KENGINE_ASYNC_TASK_STRING_MAX_LENGTH, string_name>;
		string name;

		//! putils reflect off
		std::chrono::system_clock::time_point start = std::chrono::system_clock::now();
	};
}

#include "async_task.rpp"