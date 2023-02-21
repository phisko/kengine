#pragma once

// nlohmann
#include <nlohmann/json.hpp>

// kengine functions
#include "kengine/base_function.hpp"

namespace kengine::meta {
	using load_from_json_signature = void(const nlohmann::json &, entt::handle);
	struct load_from_json : base_function<load_from_json_signature> {};
}

#define refltype kengine::meta::load_from_json
kengine_function_reflection_info;
#undef refltype