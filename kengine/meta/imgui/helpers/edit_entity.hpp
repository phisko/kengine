#pragma once

// entt
#include <entt/entity/fwd.hpp>

namespace kengine::meta::imgui {
	KENGINE_META_IMGUI_EXPORT bool edit_entity(entt::handle e) noexcept;
	KENGINE_META_IMGUI_EXPORT bool edit_entity_and_model(entt::handle e) noexcept;
}