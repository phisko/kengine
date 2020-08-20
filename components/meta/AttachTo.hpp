#pragma once

#include "BaseFunction.hpp"

namespace kengine { class Entity; }

namespace kengine::meta {
    struct AttachTo : functions::BaseFunction<
        void(Entity &)
    > {};
}

#define refltype kengine::meta::AttachTo
putils_reflection_info{
	putils_reflection_class_name;
};
#undef refltype