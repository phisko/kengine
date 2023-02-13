#pragma once

// entt
#include <entt/entity/fwd.hpp>

// kengine meta
#include "kengine/meta/functions/emplace_or_replace.hpp"

// kengine helpers
#include "kengine/meta/helpers/register_meta_component_implementation.hpp"

namespace kengine {
	template<typename T>
	struct meta_component_implementation<meta::emplace_or_replace, T> : std::true_type {
		static void function(entt::handle e, const void * comp) noexcept;
	};

	template<typename T>
	struct meta_component_implementation<meta::emplace_or_replace_move, T> : std::true_type {
		static void function(entt::handle e, void * comp) noexcept;
	};
}

#include "emplace_or_replace.inl"