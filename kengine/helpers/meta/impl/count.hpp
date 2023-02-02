#pragma once

// entt
#include <entt/entity/fwd.hpp>

// kengine meta
#include "kengine/meta/count.hpp"

// kengine helpers
#include "kengine/helpers/meta/register_meta_component_implementation.hpp"

namespace kengine {
	template<typename T>
	struct meta_component_implementation<meta::count, T> : std::true_type {
		static size_t function(entt::registry & r) noexcept;
	};
}

#include "count.inl"