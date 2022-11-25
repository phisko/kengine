#pragma once

// entt
#include <entt/entity/fwd.hpp>

// reflection
#include "putils/reflection.hpp"

namespace kengine::data {
	struct instance {
		entt::entity model = entt::null;
	};
}

#define refltype kengine::data::instance
putils_reflection_info{
	putils_reflection_class_name;
	putils_reflection_attributes(
		putils_reflection_attribute(model)
	);
};
#undef refltype