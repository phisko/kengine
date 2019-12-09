#pragma once

#include "BaseFunction.hpp"

namespace kengine { class Entity; }

namespace kengine::meta {
    struct DetachFrom : functions::BaseFunction<
        void(Entity &)
    > {
        putils_reflection_class_name(DetachFrom)
    };
} // namespace functions
