#pragma once

#ifdef _WIN32

// entt
#include <entt/entity/fwd.hpp>

namespace kengine {
	KENGINE_LOG_VISUAL_STUDIO_EXPORT void addLogVisualStudioSystem(entt::registry & r) noexcept;
}

#endif