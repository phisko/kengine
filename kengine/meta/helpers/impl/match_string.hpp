#pragma once

// entt
#include <entt/entity/fwd.hpp>

// kengine meta
#include "kengine/meta/functions/match_string.hpp"

// kengine helpers
#include "meta_component_implementation.hpp"

namespace kengine::meta {
	template<typename T>
	struct meta_component_implementation<match_string, T> : std::true_type {
		static bool function(entt::const_handle e, const char * str) noexcept;
	};
}

#include "match_string.inl"