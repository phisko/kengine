#pragma once

#ifdef _WIN32

// entt
#include <entt/entity/fwd.hpp>

namespace kengine::systems {
	KENGINE_LOG_VISUAL_STUDIO_EXPORT void add_log_visual_studio(entt::registry & r) noexcept;
}

#endif