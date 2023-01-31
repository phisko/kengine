#pragma once

// entt
#include <entt/entity/fwd.hpp>

// kengine meta
#include "kengine/meta/edit_imgui.hpp"

// kengine helpers
#include "kengine/helpers/register_meta_component_implementation.hpp"

namespace kengine {
	template<typename T>
	struct meta_component_implementation<meta::edit_imgui, T> : std::true_type {
		static void function(entt::handle e) noexcept;
	};
}

#include "edit_imgui.inl"