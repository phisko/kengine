#pragma once

// entt
#include <entt/entity/fwd.hpp>

// kengine
#include "kengine/meta/functions/get_metadata.hpp"

#include "meta_component_implementation.hpp"

namespace kengine::meta {
	template<typename T>
	struct meta_component_implementation<get_metadata, T> : std::true_type {
		static const void * function(std::string_view key) noexcept;
	};
}

#include "get_metadata.inl"