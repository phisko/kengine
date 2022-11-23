#pragma once

// entt
#include <entt/entity/fwd.hpp>

namespace kengine::mainLoop {
	KENGINE_CORE_EXPORT void run(entt::registry & r) noexcept;

	namespace timeModulated {
		KENGINE_CORE_EXPORT void run(entt::registry & r) noexcept;
	}
}