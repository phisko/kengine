#pragma once

// reflection
#include "reflection.hpp"

namespace kengine {
	struct KinematicComponent {};
}

#define refltype kengine::KinematicComponent
putils_reflection_info {
	putils_reflection_class_name;
};
#undef refltype