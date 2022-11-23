#pragma once

// putils
#include "Point.hpp"

// kengine functions
#include "BaseFunction.hpp"

namespace kengine::functions {
    struct GetPositionInPixel : BaseFunction<
        std::optional<putils::Point3f> (entt::entity window, const putils::Point2ui & pixel)
    > {};
}

#define refltype kengine::functions::GetPositionInPixel
putils_reflection_info{
	putils_reflection_class_name;
	putils_reflection_parents(
		putils_reflection_type(refltype::Base)
	);
};
#undef refltype
