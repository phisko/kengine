#pragma once

#include "sol.hpp"

namespace kengine {
	struct LuaStateComponent {
		sol::state * state = nullptr;
	};
}

#define refltype kengine::LuaStateComponent
putils_reflection_info {
	putils_reflection_class_name;
};
#undef refltype