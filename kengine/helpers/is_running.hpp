#pragma once

// entt
#include <entt/entity/fwd.hpp>

namespace kengine {
	KENGINE_CORE_EXPORT bool is_running(const entt::registry & r) noexcept;
	KENGINE_CORE_EXPORT void stop_running(entt::registry & r) noexcept;
}