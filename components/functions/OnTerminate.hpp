#pragma once

// kengine functions
#include "BaseFunction.hpp"

namespace kengine::functions {
    struct OnTerminate : BaseFunction<
        void()
    > {};
}

#define refltype kengine::functions::OnTerminate
putils_reflection_info{
	putils_reflection_class_name;
	putils_reflection_parents(
		putils_reflection_type(refltype::Base)
	);
};
#undef refltype