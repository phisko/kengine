#pragma once

// reflection
#include "reflection.hpp"

namespace kengine {
    struct TimeModulatorComponent {
        float factor = 1.f;
	};
}

#define refltype kengine::TimeModulatorComponent
putils_reflection_info {
	putils_reflection_class_name;
	putils_reflection_attributes(
		putils_reflection_attribute(factor)
	);
};
#undef refltype