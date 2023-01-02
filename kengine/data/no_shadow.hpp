#pragma once

// reflection
#include "putils/reflection.hpp"

namespace kengine::data {
	struct no_shadow {};
}

#define refltype kengine::data::no_shadow
putils_reflection_info {
	putils_reflection_class_name;
};
#undef refltype