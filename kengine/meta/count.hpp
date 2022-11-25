#pragma once

// kengine functions
#include "kengine/base_function.hpp"

namespace kengine::meta {
	struct count : functions::base_function<
		size_t()
	> {};
}

#define refltype kengine::meta::count
putils_reflection_info{
	putils_reflection_class_name;
	putils_reflection_parents(
		putils_reflection_type(refltype::base)
	);
};
#undef refltype