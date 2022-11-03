#pragma once

// reflection
#include "reflection.hpp"

// kengine
#include "Entity.hpp"

namespace kengine {
	struct InstanceComponent {
		EntityID model = INVALID_ID;
	};
}

#define refltype kengine::InstanceComponent
putils_reflection_info{
	putils_reflection_class_name;
	putils_reflection_attributes(
		putils_reflection_attribute(model)
	);
};
#undef refltype