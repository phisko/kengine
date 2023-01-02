#pragma once

// kengine functions
#include "kengine/base_function.hpp"

namespace kengine::functions {
	using on_collision_signature = void(entt::entity first, entt::entity second);
	struct on_collision : base_function<on_collision_signature> {};
}

#define refltype kengine::functions::on_collision
putils_reflection_info {
	putils_reflection_class_name;
	putils_reflection_parents(
		putils_reflection_type(refltype::base)
	);
};
#undef refltype