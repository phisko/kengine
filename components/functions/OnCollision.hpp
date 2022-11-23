#pragma once

// kengine functions
#include "BaseFunction.hpp"

namespace kengine::functions {
    struct OnCollision : BaseFunction<
        void(entt::entity first, entt::entity second)
    > {};
}

#define refltype kengine::functions::OnCollision
putils_reflection_info {
	putils_reflection_class_name;
	putils_reflection_parents(
		putils_reflection_type(refltype::Base)
	);
};
#undef refltype