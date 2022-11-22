#pragma once

// entt
#include <entt/entity/handle.hpp>

// kengine functions
#include "BaseFunction.hpp"

namespace kengine::meta {
    struct Move : functions::BaseFunction<
        void(entt::handle src, entt::handle dest)
    > {};
}

#define refltype kengine::meta::Move
putils_reflection_info{
    putils_reflection_class_name;
	putils_reflection_parents(
		putils_reflection_type(refltype::Base)
	);
};
#undef refltype