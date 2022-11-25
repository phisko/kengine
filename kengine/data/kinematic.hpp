#pragma once

// reflection
#include "putils/reflection.hpp"

namespace kengine::data {
	struct kinematic {};
}

#define refltype kengine::data::kinematic
putils_reflection_info {
	putils_reflection_class_name;
};
#undef refltype