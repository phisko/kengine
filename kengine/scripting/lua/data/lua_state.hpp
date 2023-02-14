#pragma once

#ifdef KENGINE_LUA

// sol
#include <sol/sol.hpp>

namespace kengine::data {
	//! putils reflect all
	struct lua_state {
		sol::state * state = nullptr;
	};
}

#include "lua_state.rpp"

#endif