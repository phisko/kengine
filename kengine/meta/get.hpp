#pragma once

// kengine functions
#include "kengine/base_function.hpp"

namespace kengine::meta {
    struct get : functions::base_function<
        void * (entt::handle)
	>{};

	struct get_const : functions::base_function<
		const void * (entt::const_handle)
	>{};
}

#define refltype kengine::meta::get
putils_reflection_info{
    putils_reflection_class_name;
    putils_reflection_parents(
        putils_reflection_type(refltype::base)
	);
};
#undef refltype