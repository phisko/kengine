#pragma once

// entt
#include <entt/entity/fwd.hpp>

namespace kengine::systems {
	KENGINE_IMGUI_ENGINE_STATS_EXPORT entt::entity add_imgui_engine_stats(entt::registry & r) noexcept;
}