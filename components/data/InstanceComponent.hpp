#pragma once

// entt
#include <entt/entity/fwd.hpp>

// reflection
#include "reflection.hpp"

namespace kengine {
	struct InstanceComponent {
		entt::entity model = entt::null;
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