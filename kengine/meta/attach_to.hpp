#pragma once

// kengine functions
#include "kengine/base_function.hpp"

namespace kengine::meta {
	using attach_to_signature = void(entt::handle);
	struct attach_to : functions::base_function<attach_to_signature> {};
}

#define refltype kengine::meta::attach_to
putils_reflection_info {
	putils_reflection_class_name;
	putils_reflection_parents(
		putils_reflection_type(refltype::base)
	);
};
#undef refltype