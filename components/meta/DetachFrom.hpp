#pragma once

// kengine functions
#include "BaseFunction.hpp"

namespace kengine { class Entity; }

namespace kengine::meta {
    struct DetachFrom : functions::BaseFunction<
        void(Entity &)
    > {};
}

#define refltype kengine::meta::DetachFrom
putils_reflection_info {
	putils_reflection_class_name
	putils_reflection_parents(
		putils_reflection_type(refltype::Base)
	);
};
#undef refltype
