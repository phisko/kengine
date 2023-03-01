#pragma once

// sol
#include <sol/sol.hpp>

namespace kengine::scripting::lua {
	//! putils reflect all
	//! class_name: lua_state
	struct state {
		sol::state * ptr = nullptr;
	};
}

#include "state.rpp"