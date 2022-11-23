#pragma once

// entt
#include <entt/entity/fwd.hpp>

namespace kengine {
	KENGINE_CORE_EXPORT bool isRunning(const entt::registry & r) noexcept;
	KENGINE_CORE_EXPORT void stopRunning(entt::registry & r) noexcept;
}