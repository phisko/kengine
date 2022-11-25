#pragma once

// kengine functions
#include "kengine/base_function.hpp"

namespace kengine::functions {
    struct execute : base_function<
        void(float delta_time)
    > {};
}

#define refltype kengine::functions::execute
putils_reflection_info{
    putils_reflection_class_name;
	putils_reflection_parents(
		putils_reflection_type(refltype::base)
	);
};
#undef refltype