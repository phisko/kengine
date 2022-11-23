#pragma once

// kengine functions
#include "BaseFunction.hpp"

namespace kengine::meta {
    struct Get : functions::BaseFunction<
        void * (entt::handle)
	>{};

	struct GetConst : functions::BaseFunction<
		const void * (entt::const_handle)
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