#pragma once

// entt
#include <entt/entity/fwd.hpp>

// kengine meta
#include "kengine/meta/imgui/functions/edit_imgui.hpp"

// kengine helpers
#include "kengine/meta/helpers/impl/meta_component_implementation.hpp"

namespace kengine::meta {
	template<typename T>
	struct meta_component_implementation<edit_imgui, T> : std::true_type {
		static void function(entt::handle e) noexcept;
	};
}

#include "edit_imgui.inl"