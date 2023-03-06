#pragma once

// entt
#include <entt/entity/fwd.hpp>

// kengine
#include "kengine/meta/json/functions/save.hpp"
#include "kengine/meta/helpers/impl/meta_component_implementation.hpp"

namespace kengine::meta {
	template<typename T>
	struct meta_component_implementation<json::save, T> : std::true_type {
		static nlohmann::json function(entt::const_handle e) noexcept;
	};
}

#include "save.inl"