#pragma once

// kengine functions
#include "kengine/base_function.hpp"

namespace kengine::meta {
	using has_signature = bool(entt::const_handle);
	struct has : functions::base_function<has_signature> {};
}

#define refltype kengine::meta::has
kengine_function_reflection_info;
#undef refltype