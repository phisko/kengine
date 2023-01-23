#pragma once

// entt
#include <entt/entity/fwd.hpp>

namespace kengine::systems {
	KENGINE_IMGUI_ENTITY_EDITOR_EXPORT entt::entity add_imgui_entity_editor(entt::registry & r) noexcept;
}