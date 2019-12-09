#pragma once

#include "BaseFunction.hpp"

namespace kengine { class Entity; }

namespace kengine::meta {
    struct AttachTo : functions::BaseFunction<
        void(Entity &)
    > {
        putils_reflection_class_name(AttachTo)
    };
} // namespace functions