#pragma once

// entt
#include <entt/entity/fwd.hpp>

// kengine
#include "kengine/meta/functions/get.hpp"

#include "meta_component_implementation.hpp"

namespace kengine::meta {
	template<typename T>
	struct meta_component_implementation<get, T> : std::true_type {
		static void * function(entt::handle e) noexcept;
	};

	template<typename T>
	struct meta_component_implementation<get_const, T> : std::true_type {
		static const void * function(entt::const_handle e) noexcept;
	};
}

#include "get.inl"