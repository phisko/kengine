#pragma once

#ifdef KENGINE_LUA

// sol
#include <sol/sol.hpp>

// reflection
#include "putils/reflection.hpp"

namespace kengine::data {
	struct lua_state {
		sol::state * state = nullptr;
	};
}

#define refltype kengine::data::lua_state
putils_reflection_info {
	putils_reflection_class_name;
};
#undef refltype

#endif