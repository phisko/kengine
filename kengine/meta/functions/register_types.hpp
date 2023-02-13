#pragma once

// entt
#include <entt/entity/fwd.hpp>

// kengine functions
#include "kengine/base_function.hpp"

namespace kengine::functions {
	using register_types_signature = void(entt::registry & r);

	struct pre_register_types : base_function<register_types_signature> {};

	struct register_types : base_function<register_types_signature> {};
}

#define refltype kengine::functions::pre_register_types
kengine_function_reflection_info;
#undef refltype

#define refltype kengine::functions::register_types
kengine_function_reflection_info;
#undef refltype