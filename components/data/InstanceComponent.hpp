#pragma once

#include "Entity.hpp"
#include "reflection.hpp"

namespace kengine {
	struct InstanceComponent {
		Entity::ID model = Entity::INVALID_ID;
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