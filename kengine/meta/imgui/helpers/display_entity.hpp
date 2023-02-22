#pragma once

// entt
#include <entt/entity/fwd.hpp>

namespace kengine::meta::imgui {
	KENGINE_META_IMGUI_EXPORT void display_entity(entt::const_handle e) noexcept;
	KENGINE_META_IMGUI_EXPORT void display_entity_and_model(entt::const_handle e) noexcept;
}