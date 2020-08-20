#pragma once

#include "BaseFunction.hpp"

namespace kengine { class Entity; }

namespace kengine::functions {
    struct OnCollision : BaseFunction<
        void(Entity & first, Entity & second)
    > {};
}

#define refltype kengine::functions::OnCollision
putils_reflection_info {
	putils_reflection_class_name;
};
#undef refltype