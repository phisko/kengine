#pragma once

// kengine functions
#include "kengine/base_function.hpp"

namespace kengine::meta {
	using copy_signature = void(entt::const_handle src, entt::handle dest);
	struct copy : functions::base_function<copy_signature> {};
}

#define refltype kengine::meta::copy
putils_reflection_info {
	putils_reflection_class_name;
	putils_reflection_parents(
		putils_reflection_type(refltype::base)
	);
};
#undef refltype