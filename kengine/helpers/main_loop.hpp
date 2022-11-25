#pragma once

// entt
#include <entt/entity/fwd.hpp>

namespace kengine::main_loop {
	KENGINE_CORE_EXPORT void run(entt::registry & r) noexcept;

	namespace time_modulated {
		KENGINE_CORE_EXPORT void run(entt::registry & r) noexcept;
	}
}