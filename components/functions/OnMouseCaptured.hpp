#pragma once

#include "BaseFunction.hpp"
#include "Entity.hpp"

namespace kengine::functions {
    struct OnMouseCaptured : BaseFunction<
        void(Entity::ID window, bool captured)
    > {
        putils_reflection_class_name(OnMouseCaptured);
    };
}