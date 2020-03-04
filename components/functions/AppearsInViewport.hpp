#pragma once

#include "BaseFunction.hpp"
#include "Entity.hpp"

namespace kengine::functions {
	struct AppearsInViewport : BaseFunction<
		bool(Entity::ID)
	> {
		putils_reflection_class_name(AppearsInViewport);
	};
}