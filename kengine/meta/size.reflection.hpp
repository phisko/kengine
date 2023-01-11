#pragma once

#include "putils/reflection.hpp"

#define refltype kengine::meta::size
putils_reflection_info {
	putils_reflection_class_name;
	putils_reflection_attributes(
		putils_reflection_attribute(size)
	);
};
#undef refltype