#pragma once

#include "BaseFunction.hpp"

namespace kengine { class Entity; }

namespace kengine::meta {
    struct Has : functions::BaseFunction<
        bool(const Entity &)
    > {
        putils_reflection_class_name(Has)
    };
} // namespace functions