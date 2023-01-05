#pragma once

// kengine functions
#include "kengine/base_function.hpp"

namespace kengine::functions {
	using on_collision_signature = void(entt::entity first, entt::entity second);
	struct on_collision : base_function<on_collision_signature> {};
}

#define refltype kengine::functions::on_collision
kengine_function_reflection_info;
#undef refltype