#pragma once

// entt
#include <entt/entity/fwd.hpp>

// kengine meta
#include "kengine/meta/match_string.hpp"

// kengine helpers
#include "kengine/helpers/register_meta_component_implementation.hpp"

namespace kengine {
	template<typename T>
	struct meta_component_implementation<meta::match_string, T> : std::true_type {
		static bool function(entt::const_handle e, const char * str) noexcept;
	};
}

#include "match_string.inl"