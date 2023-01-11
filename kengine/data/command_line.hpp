#pragma once

// stl
#include <vector>
#include <string>

namespace kengine::data {
	//! putils reflect all
	struct command_line {
		std::vector<std::string_view> arguments;
	};
}

#include "command_line.reflection.hpp"