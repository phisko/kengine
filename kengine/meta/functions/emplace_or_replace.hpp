#pragma once

// kengine functions
#include "kengine/base_function.hpp"

namespace kengine::meta {
	using emplace_or_replace_signature = void(entt::handle, const void * comp);
	struct emplace_or_replace : base_function<emplace_or_replace_signature> {};

	using emplace_or_replace_move_signature = void(entt::handle, void * comp);
	struct emplace_or_replace_move : base_function<emplace_or_replace_move_signature> {};
}

#define refltype kengine::meta::emplace_or_replace
kengine_function_reflection_info;
#undef refltype

#define refltype kengine::meta::emplace_or_replace_move
kengine_function_reflection_info;
#undef refltype