#pragma once

// entt
#include <entt/entity/fwd.hpp>

// kengine meta
#include "kengine/meta/functions/has.hpp"

// kengine helpers
#include "meta_component_implementation.hpp"

namespace kengine::meta {
	template<typename T>
	struct meta_component_implementation<has, T> : std::true_type {
		static bool function(entt::const_handle e) noexcept;
	};
}

#include "has.inl"