#pragma once

// entt
#include <entt/entity/fwd.hpp>

namespace kengine {
	KENGINE_MAIN_LOOP_EXPORT bool is_running(const entt::registry & r) noexcept;
	KENGINE_MAIN_LOOP_EXPORT void stop_running(entt::registry & r) noexcept;
}