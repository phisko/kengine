#pragma once

#include "BaseFunction.hpp"
#include "Entity.hpp"
#include "Point.hpp"

namespace kengine::functions {
    struct GetEntityInPixel : BaseFunction<
        Entity::ID(Entity::ID window, const putils::Point2ui & pixel)
    > {
        putils_reflection_class_name(GetEntityInPixel);
    };
}