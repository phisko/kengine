#pragma once

// kengine functions
#include "kengine/base_function.hpp"

namespace kengine::meta {
    struct copy : functions::base_function<
        void(entt::const_handle src, entt::handle dest)
    > {};
}

#define refltype kengine::meta::copy
putils_reflection_info{
    putils_reflection_class_name;
	putils_reflection_parents(
		putils_reflection_type(refltype::base)
	);
};
#undef refltype