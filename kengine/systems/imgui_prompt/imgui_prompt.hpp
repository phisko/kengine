#pragma once

// entt
#include <entt/entity/fwd.hpp>

namespace kengine::systems {
	KENGINE_IMGUI_PROMPT_EXPORT entt::entity add_imgui_prompt(entt::registry & r) noexcept;
}