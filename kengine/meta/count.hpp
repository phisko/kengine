#pragma once

// kengine functions
#include "kengine/base_function.hpp"

namespace kengine::meta {
	using count_signature = size_t();
	struct count : functions::base_function<count_signature> {};
}

#define refltype kengine::meta::count
putils_reflection_info {
	putils_reflection_class_name;
	putils_reflection_parents(
		putils_reflection_type(refltype::base)
	);
};
#undef refltype