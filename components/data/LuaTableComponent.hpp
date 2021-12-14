#pragma once

#include "lua/sol.hpp"
#include "reflection.hpp"

namespace kengine {
	struct LuaTableComponent {
		sol::table table;
	};
}

#define refltype kengine::LuaTableComponent
putils_reflection_info {
	putils_reflection_class_name;
	putils_reflection_attributes(
		putils_reflection_attribute(table)
	);
};
#undef refltype