#pragma once

// entt
#include <entt/entity/fwd.hpp>

namespace kengine::systems {
	KENGINE_IMGUI_ENTITY_SELECTOR_EXPORT entt::entity add_imgui_entity_selector(entt::registry & r) noexcept;
}