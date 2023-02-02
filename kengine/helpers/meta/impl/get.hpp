#pragma once

// entt
#include <entt/entity/fwd.hpp>

// kengine meta
#include "kengine/meta/get.hpp"

// kengine helpers
#include "kengine/helpers/meta/register_meta_component_implementation.hpp"

namespace kengine {
	template<typename T>
	struct meta_component_implementation<meta::get, T> : std::true_type {
		static void * function(entt::handle e) noexcept;
	};

	template<typename T>
	struct meta_component_implementation<meta::get_const, T> : std::true_type {
		static const void * function(entt::const_handle e) noexcept;
	};
}

#include "get.inl"