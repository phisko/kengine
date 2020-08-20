#pragma once

#include "reflection.hpp"

namespace kengine {
	struct DefaultShadowComponent {};
}

#define refltype kengine::DefaultShadowComponent
putils_reflection_info{
	putils_reflection_class_name;
};
#undef refltype
