#pragma once

#include "putils/reflection.hpp"

#define refltype kengine::data::lua_state
putils_reflection_info {
	putils_reflection_class_name;
	putils_reflection_attributes(
		putils_reflection_attribute(state)
	);
};
#undef refltype