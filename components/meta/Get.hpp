#pragma once

// kengine functions
#include "BaseFunction.hpp"

namespace kengine { class Entity; }

namespace kengine::meta {
    struct Get : functions::BaseFunction<
        void * (const Entity &)
	>{};
}

#define refltype kengine::meta::Get
putils_reflection_info{
    putils_reflection_class_name;
    putils_reflection_parents(
        putils_reflection_type(refltype::Base)
	);
};
#undef refltype