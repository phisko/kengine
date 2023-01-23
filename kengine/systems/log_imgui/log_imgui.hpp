#pragma once

// entt
#include <entt/entity/fwd.hpp>

namespace kengine::systems {
	KENGINE_LOG_IMGUI_EXPORT entt::entity add_log_imgui(entt::registry & r) noexcept;
}