#pragma once

// entt
#include <entt/entity/fwd.hpp>

namespace kengine::main_loop {
	KENGINE_MAIN_LOOP_EXPORT void run(entt::registry & r) noexcept;

	namespace time_modulated {
		KENGINE_MAIN_LOOP_EXPORT void run(entt::registry & r) noexcept;
	}
}