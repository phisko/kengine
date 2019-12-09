#pragma once

#include "BaseFunction.hpp"

namespace kengine { class Entity; }

namespace kengine::functions {
    struct OnEntityCreated : BaseFunction<
        void(Entity & e)
    > {
        putils_reflection_class_name(OnEntityCreated);
    };
}