#pragma once

#include "lua/sol.hpp"

namespace kengine {
	struct LuaTableComponent {
		sol::table table;

		putils_reflection_class_name(LuaTableComponent);
		putils_reflection_attributes(
			putils_reflection_attribute(&LuaTableComponent::table)
		);
	};
}