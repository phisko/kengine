#pragma once

// kengine functions
#include "kengine/base_function.hpp"

namespace kengine::meta {
	using has_signature = bool(entt::const_handle);
	struct has : functions::base_function<has_signature> {};
}

#define refltype kengine::meta::has
putils_reflection_info {
	putils_reflection_class_name;
	putils_reflection_parents(
		putils_reflection_type(refltype::base)
	);
};
#undef refltype