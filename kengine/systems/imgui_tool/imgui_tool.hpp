#pragma once

// entt
#include <entt/entity/fwd.hpp>

namespace kengine::systems {
	KENGINE_IMGUI_TOOL_EXPORT entt::entity add_imgui_tool(entt::registry & r) noexcept;
}