#pragma once

// entt
#include <entt/entity/fwd.hpp>

// kengine meta
#include "kengine/meta/display_imgui.hpp"

// kengine helpers
#include "kengine/helpers/meta/register_meta_component_implementation.hpp"

namespace kengine {
	template<typename T>
	struct meta_component_implementation<meta::display_imgui, T> : std::true_type {
		static void function(entt::const_handle e) noexcept;
	};
}

#include "display_imgui.inl"