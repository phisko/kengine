#include "is_running.hpp"

// entt
#include <entt/entity/registry.hpp>

// kengine core
#include "kengine/core/log/helpers/kengine_log.hpp"

// kengine main_loop
#include "kengine/main_loop/data/keep_alive.hpp"

namespace kengine::main_loop {
	bool is_running(const entt::registry & r) noexcept {
		kengine_log(r, very_verbose, "main_loop", "Checking for main_loop::keep_alive");
		return !r.view<keep_alive>().empty();
	}
}