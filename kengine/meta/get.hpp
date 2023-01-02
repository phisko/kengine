#pragma once

// kengine functions
#include "kengine/base_function.hpp"

namespace kengine::meta {
	using get_signature = void *(entt::handle);
	struct get : functions::base_function<get_signature> {};

	using get_const_signature = const void *(entt::const_handle);
	struct get_const : functions::base_function<get_const_signature> {};
}

#define refltype kengine::meta::get
putils_reflection_info {
	putils_reflection_class_name;
	putils_reflection_parents(
		putils_reflection_type(refltype::base)
	);
};
#undef refltype

#define refltype kengine::meta::get_const
putils_reflection_info {
	putils_reflection_class_name;
	putils_reflection_parents(
		putils_reflection_type(refltype::base)
	);
};
#undef refltype