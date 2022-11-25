#pragma once

#ifdef KENGINE_LUA

// sol
#include <sol/sol.hpp>

// reflection
#include "putils/reflection.hpp"

namespace kengine::data {
	struct lua_table {
		sol::table table;
	};
}

#define refltype kengine::data::lua_table
putils_reflection_info {
	putils_reflection_class_name;
	putils_reflection_attributes(
		putils_reflection_attribute(table)
	);
};
#undef refltype

#endif