#pragma once

// entt
#include <entt/entity/fwd.hpp>

namespace kengine::systems {
	KENGINE_IMGUI_ADJUSTABLE_EXPORT entt::entity add_imgui_adjustable(entt::registry & r) noexcept;
}