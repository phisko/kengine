#pragma once

// entt
#include <entt/entity/fwd.hpp>

namespace kengine::imgui_helper {
	KENGINE_IMGUI_EXPORT void display_entity(entt::const_handle e) noexcept;
	KENGINE_IMGUI_EXPORT void display_entity_and_model(entt::const_handle e) noexcept;
	KENGINE_IMGUI_EXPORT void edit_entity(entt::handle e) noexcept;
	KENGINE_IMGUI_EXPORT void edit_entity_and_model(entt::handle e) noexcept;
}