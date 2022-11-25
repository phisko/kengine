#pragma once

// kengine functions
#include "kengine/base_function.hpp"

namespace kengine::meta {
	struct match_string : functions::base_function<
		bool(entt::const_handle, const char *)
	> {};
}

#define refltype kengine::meta::match_string
putils_reflection_info {
	putils_reflection_class_name;
	putils_reflection_parents(
		putils_reflection_type(refltype::base)
	);
};
#undef refltype