#pragma once

// kengine functions
#include "BaseFunction.hpp"

namespace kengine::functions {
    struct OnMouseCaptured : BaseFunction<
        void(entt::entity window, bool captured)
    > {};
}

#define refltype kengine::functions::OnMouseCaptured
putils_reflection_info{
	putils_reflection_class_name;
	putils_reflection_parents(
		putils_reflection_type(refltype::Base)
	);
};
#undef refltype