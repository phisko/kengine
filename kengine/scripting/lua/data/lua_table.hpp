#pragma once

// sol
#include <sol/sol.hpp>

namespace kengine::data {
	//! putils reflect all
	struct lua_table {
		sol::table table;
	};
}

#include "lua_table.rpp"