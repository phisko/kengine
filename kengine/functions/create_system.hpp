#pragma once

// kengine functions
#include "kengine/base_function.hpp"

namespace kengine::functions {
	using create_system_signature = entt::entity(entt::registry &);
	struct create_system : base_function<create_system_signature> {};
}

#define refltype kengine::functions::create_system
kengine_function_reflection_info;
#undef refltype
