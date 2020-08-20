#pragma once

#include "BaseFunction.hpp"
#include "Entity.hpp"

namespace kengine::functions {
	struct AppearsInViewport : BaseFunction<
		bool(Entity::ID)
	> {};
}

#define refltype kengine::functions::AppearsInViewport
putils_reflection_info{
	putils_reflection_class_name;
};
#undef refltype