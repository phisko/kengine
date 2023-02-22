#pragma once

// nlohmann
#include <nlohmann/json.hpp>

// kengine functions
#include "kengine/base_function.hpp"

namespace kengine::meta::json {
	using load_signature = void(const nlohmann::json &, entt::handle);
	struct load : base_function<load_signature> {};
}

#define refltype kengine::meta::json::load
kengine_function_reflection_info;
#undef refltype