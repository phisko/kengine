#pragma once

// reflection
#include "reflection.hpp"

namespace kengine {
	struct KeepAlive {};
}

#define refltype kengine::KeepAlive
putils_reflection_info {
	putils_reflection_class_name;
};
#undef refltype