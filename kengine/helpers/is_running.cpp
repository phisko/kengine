#include "is_running.hpp"

// entt
#include <entt/entity/registry.hpp>

// kengine data
#include "kengine/data/keep_alive.hpp"

namespace kengine {
	bool is_running(const entt::registry & r) noexcept {
		return !r.view<data::keep_alive>().empty();
	}

	void stop_running(entt::registry & r) noexcept {
		r.clear<data::keep_alive>();
	}
}