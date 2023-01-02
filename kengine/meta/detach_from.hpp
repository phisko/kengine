#pragma once

// kengine functions
#include "kengine/base_function.hpp"

namespace kengine::meta {
	using detach_from_signature = void(entt::handle);
	struct detach_from : functions::base_function<detach_from_signature> {};
}

#define refltype kengine::meta::detach_from
putils_reflection_info {
	putils_reflection_class_name;
	putils_reflection_parents(
		putils_reflection_type(refltype::base)
	);
};
#undef refltype
