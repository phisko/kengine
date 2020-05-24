#pragma once

#include "Entity.hpp"
#include "reflection.hpp"

namespace kengine {
	struct InstanceComponent {
		Entity::ID model = Entity::INVALID_ID;

		putils_reflection_class_name(InstanceComponent);
		putils_reflection_attributes(
			putils_reflection_attribute(&InstanceComponent::model)
		);
	};
}