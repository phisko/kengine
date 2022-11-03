#pragma once

// reflection
#include "reflection.hpp"

namespace kengine {
	struct NoShadowComponent {};
}

#define refltype kengine::NoShadowComponent
putils_reflection_info{
	putils_reflection_class_name;
};
#undef refltype