#pragma once

// entt
#include <entt/entity/fwd.hpp>

namespace kengine::main_loop {
	KENGINE_MAIN_LOOP_EXPORT void stop_running(entt::registry & r) noexcept;
}