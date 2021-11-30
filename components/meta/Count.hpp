#pragma once

#include "BaseFunction.hpp"

namespace kengine::meta {
	struct Count : functions::BaseFunction<
		size_t()
	> {};
}

#define refltype kengine::meta::Count
putils_reflection_info{
	putils_reflection_class_name;
	putils_reflection_parents(
		putils_reflection_type(refltype::Base)
	);
};
#undef refltype