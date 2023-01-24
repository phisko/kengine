#pragma once

// entt
#include <entt/entity/fwd.hpp>

namespace kengine::systems {
	KENGINE_IMGUI_ASYNC_TASK_EXPORT entt::entity add_imgui_async_task(entt::registry & r) noexcept;
}