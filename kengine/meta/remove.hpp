#pragma once

// kengine functions
#include "kengine/base_function.hpp"

namespace kengine::meta {
	using remove_signature = void(entt::handle);
	struct remove : functions::base_function<remove_signature> {};
}

#define refltype kengine::meta::remove
putils_reflection_info {
	putils_reflection_class_name;
	putils_reflection_parents(
		putils_reflection_type(refltype::base)
	);
};
#undef refltype
