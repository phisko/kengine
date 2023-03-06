#pragma once

// entt
#include <entt/entity/fwd.hpp>

// kengine
#include "kengine/meta/functions/count.hpp"

#include "meta_component_implementation.hpp"

namespace kengine::meta {
	template<typename T>
	struct meta_component_implementation<count, T> : std::true_type {
		static size_t function(entt::registry & r) noexcept;
	};
}

#include "count.inl"