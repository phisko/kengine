#pragma once

// entt
#include <entt/entity/fwd.hpp>

// kengine meta
#include "kengine/meta/functions/save_to_json.hpp"

// kengine helpers
#include "kengine/meta/helpers/register_meta_component_implementation.hpp"

namespace kengine {
	template<typename T>
	struct meta_component_implementation<meta::save_to_json, T> : std::true_type {
		static nlohmann::json function(entt::const_handle e) noexcept;
	};
}

#include "save_to_json.inl"