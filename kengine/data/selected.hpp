#pragma once

// reflection
#include "putils/reflection.hpp"

namespace kengine::data {
	struct selected {};
}

#define refltype kengine::data::selected
putils_reflection_info {
	putils_reflection_class_name;
};
#undef refltype