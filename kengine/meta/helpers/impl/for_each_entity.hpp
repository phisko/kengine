#pragma once

// entt
#include <entt/entity/fwd.hpp>

// kengine meta
#include "kengine/meta/functions/for_each_entity.hpp"

// kengine helpers
#include "meta_component_implementation.hpp"

namespace kengine::meta {
	template<typename T>
	struct meta_component_implementation<for_each_entity, T> : std::true_type {
		static void function(entt::registry & r, const entity_iterator_func & func) noexcept;
	};

	template<typename T>
	struct meta_component_implementation<for_each_entity_without, T> : std::true_type {
		static void function(entt::registry & r, const entity_iterator_func & func) noexcept;
	};
}

#include "for_each_entity.inl"