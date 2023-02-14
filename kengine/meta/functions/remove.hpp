#pragma once

// kengine functions
#include "kengine/base_function.hpp"

namespace kengine::meta {
	using remove_signature = void(entt::handle);
	struct remove : functions::base_function<remove_signature> {};
}

#define refltype kengine::meta::remove
kengine_function_reflection_info;
#undef refltype
