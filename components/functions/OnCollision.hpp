#pragma once

#include "BaseFunction.hpp"

namespace kengine { class Entity; }

namespace kengine::functions {
    struct OnCollision : BaseFunction<
        void(Entity & first, Entity & second)
    > {
        putils_reflection_class_name(OnCollision);
    };
}