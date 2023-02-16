#pragma once

// stl
#include <vector>
#include <string>

namespace kengine::command_line {
	//! putils reflect all
	struct arguments {
		std::vector<std::string_view> args;
	};
}

#include "arguments.rpp"