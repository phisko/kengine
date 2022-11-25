#pragma once

// reflection
#include "putils/reflection.hpp"

namespace kengine::data {
    struct time_modulator {
        float factor = 1.f;
	};
}

#define refltype kengine::data::time_modulator
putils_reflection_info {
	putils_reflection_class_name;
	putils_reflection_attributes(
		putils_reflection_attribute(factor)
	);
};
#undef refltype