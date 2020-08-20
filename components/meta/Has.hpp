#pragma once

#include "BaseFunction.hpp"

namespace kengine { class Entity; }

namespace kengine::meta {
    struct Has : functions::BaseFunction<
        bool(const Entity &)
    > {};
}

#define refltype kengine::meta::Has
putils_reflection_info{
    putils_reflection_class_name;
};
#undef refltype