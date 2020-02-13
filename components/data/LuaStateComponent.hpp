#pragma once

#include "sol.hpp"

namespace kengine {
	struct LuaStateComponent {
		sol::state * state = nullptr;
		putils_reflection_class_name(LuaStateComponent);
	};
}