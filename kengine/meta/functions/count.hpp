#pragma once

// entt
#include <entt/entity/fwd.hpp>

// kengine functions
#include "kengine/base_function.hpp"

namespace kengine::meta {
	using count_signature = size_t(entt::registry &);
	struct count : functions::base_function<count_signature> {};
}

#define refltype kengine::meta::count
kengine_function_reflection_info;
#undef refltype