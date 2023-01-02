#pragma once

// entt
#include <entt/entity/handle.hpp>

// kengine functions
#include "kengine/base_function.hpp"

namespace kengine::meta {
	using move_signature = void(entt::handle src, entt::handle dest);
	struct move : functions::base_function<move_signature> {};
}

#define refltype kengine::meta::move
putils_reflection_info {
	putils_reflection_class_name;
	putils_reflection_parents(
		putils_reflection_type(refltype::base)
	);
};
#undef refltype