#pragma once

// kengine functions
#include "kengine/base_function.hpp"

namespace kengine::functions {
	using appears_in_viewport_signature = bool(entt::entity entity);
	struct appears_in_viewport : base_function<appears_in_viewport_signature> {};
}

#define refltype kengine::functions::appears_in_viewport
putils_reflection_info {
	putils_reflection_class_name;
	putils_reflection_parents(
		putils_reflection_type(refltype::base)
	);
};
#undef refltype