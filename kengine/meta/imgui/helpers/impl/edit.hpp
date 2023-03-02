#pragma once

// entt
#include <entt/entity/fwd.hpp>

// kengine meta
#include "kengine/meta/imgui/functions/edit.hpp"

// kengine helpers
#include "kengine/meta/helpers/impl/meta_component_implementation.hpp"

namespace kengine::meta {
	template<typename T>
	struct meta_component_implementation<imgui::edit, T> : std::true_type {
		static bool function(entt::handle e) noexcept;
	};
}

#include "edit.inl"