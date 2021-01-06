#pragma once

#include "BaseFunction.hpp"
#include "impl/ID.hpp"
#include "Point.hpp"

namespace kengine::functions {
    struct GetPositionInPixel : BaseFunction<
        putils::Point3f(EntityID window, const putils::Point2ui & pixel)
    > {};
}

#define refltype kengine::functions::GetPositionInPixel
putils_reflection_info{
	putils_reflection_class_name;
};
#undef refltype
