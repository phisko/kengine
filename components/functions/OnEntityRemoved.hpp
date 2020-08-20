#pragma once

#include "BaseFunction.hpp"

namespace kengine { class Entity; }

namespace kengine::functions {
    struct OnEntityRemoved : BaseFunction<
        void(Entity & e)
    > {};
}

#define refltype kengine::functions::OnEntityRemoved
putils_reflection_info{
    putils_reflection_class_name;
};
#undef refltype