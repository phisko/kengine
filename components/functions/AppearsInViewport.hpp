#pragma once

// kengine functions
#include "BaseFunction.hpp"

namespace kengine::functions {
	struct AppearsInViewport : BaseFunction<
		bool(entt::entity entity)
	> {};
}

#define refltype kengine::functions::AppearsInViewport
putils_reflection_info{
	putils_reflection_class_name;
	putils_reflection_parents(
		putils_reflection_type(refltype::Base)
	);
};
#undef refltype