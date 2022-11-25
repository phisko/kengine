#pragma once

// reflection
#include "putils/reflection.hpp"

namespace kengine::data {
	struct keep_alive {};
}

#define refltype kengine::data::keep_alive
putils_reflection_info {
	putils_reflection_class_name;
};
#undef refltype