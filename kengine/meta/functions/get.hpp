#pragma once

// kengine functions
#include "kengine/base_function.hpp"

namespace kengine::meta {
	using get_signature = void *(entt::handle);
	struct get : base_function<get_signature> {};

	using get_const_signature = const void *(entt::const_handle);
	struct get_const : base_function<get_const_signature> {};
}

#define refltype kengine::meta::get
kengine_function_reflection_info;
#undef refltype

#define refltype kengine::meta::get_const
kengine_function_reflection_info;
#undef refltype