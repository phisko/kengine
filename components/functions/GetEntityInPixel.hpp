#pragma once

#include "Point.hpp"

// kengine functions
#include "BaseFunction.hpp"

namespace kengine::functions {
    struct GetEntityInPixel : BaseFunction<
        entt::entity(entt::entity window, const putils::Point2ui & pixel)
    > {};
}

#define refltype kengine::functions::GetEntityInPixel
putils_reflection_info{
    putils_reflection_class_name;
	putils_reflection_parents(
		putils_reflection_type(refltype::Base)
	);
};
#undef refltype