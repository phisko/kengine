#pragma once

// nlohmann
#include <nlohmann/json.hpp>

// kengine functions
#include "kengine/base_function.hpp"

namespace kengine::meta::json {
	using save_signature = nlohmann::json(entt::const_handle);
	struct save : base_function<save_signature> {};
}

#define refltype kengine::meta::json::save
kengine_function_reflection_info;
#undef refltype
