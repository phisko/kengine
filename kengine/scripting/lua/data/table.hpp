#pragma once

// sol
#include <sol/sol.hpp>

namespace kengine::scripting::lua {
	//! putils reflect all
	//! class_name: lua_table
	struct table {
		sol::table value;
	};
}

#include "table.rpp"