#pragma once

// entt
#include <entt/entity/fwd.hpp>

// kengine
#include "kengine/meta/functions/has_metadata.hpp"

#include "meta_component_implementation.hpp"

namespace kengine::meta {
	template<typename T>
	struct meta_component_implementation<has_metadata, T> : std::true_type {
		static bool function(std::string_view key) noexcept;
	};
}

#include "has_metadata.inl"