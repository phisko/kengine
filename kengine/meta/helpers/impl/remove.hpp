#pragma once

// entt
#include <entt/entity/fwd.hpp>

// kengine meta
#include "kengine/meta/functions/remove.hpp"

// kengine helpers
#include "meta_component_implementation.hpp"

namespace kengine::meta {
	template<typename T>
	struct meta_component_implementation<remove, T> : std::true_type {
		static void function(entt::handle e) noexcept;
	};
}

#include "remove.inl"