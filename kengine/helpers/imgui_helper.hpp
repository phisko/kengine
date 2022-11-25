#pragma once

// entt
#include <entt/entity/fwd.hpp>

namespace kengine::imgui_helper {
	KENGINE_CORE_EXPORT void display_entity(entt::const_handle e) noexcept;
	KENGINE_CORE_EXPORT void display_entity_and_model(entt::const_handle e) noexcept;
	KENGINE_CORE_EXPORT void edit_entity(entt::handle e) noexcept;
	KENGINE_CORE_EXPORT void edit_entity_and_model(entt::handle e) noexcept;
	KENGINE_CORE_EXPORT float get_scale(const entt::registry & r) noexcept;
}