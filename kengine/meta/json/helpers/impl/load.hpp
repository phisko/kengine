#pragma once

// stl
#include <type_traits>

// entt
#include <entt/entity/fwd.hpp>

// kengine meta
#include "kengine/meta/json/functions/load.hpp"

// kengine helpers
#include "kengine/meta/helpers/register_meta_component_implementation.hpp"

namespace kengine::meta {
	template<typename T>
	struct meta_component_implementation<json::load, T> {
		static constexpr bool value = std::is_move_assignable_v<T>;
		static void function(const nlohmann::json & json_entity, entt::handle e) noexcept;
	};
}

#include "load.inl"