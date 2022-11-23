#pragma once

// kengine functions
#include "BaseFunction.hpp"

namespace kengine::meta {
    struct Copy : functions::BaseFunction<
        void(entt::const_handle src, entt::handle dest)
    > {};
}

#define refltype kengine::meta::Copy
putils_reflection_info{
    putils_reflection_class_name;
	putils_reflection_parents(
		putils_reflection_type(refltype::Base)
	);
};
#undef refltype