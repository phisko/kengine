#include "stop_running.hpp"

// entt
#include <entt/entity/registry.hpp>

// kengine
#include "kengine/core/log/helpers/kengine_log.hpp"
#include "kengine/main_loop/data/keep_alive.hpp"

namespace kengine::main_loop {
	void stop_running(entt::registry & r) noexcept {
		kengine_log(r, verbose, "main_loop", "Removing all main_loop::keep_alive");
		r.clear<keep_alive>();
	}
}