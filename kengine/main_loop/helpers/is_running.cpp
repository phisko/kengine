#include "is_running.hpp"

// entt
#include <entt/entity/registry.hpp>

// kengine data
#include "kengine/data/keep_alive.hpp"

// kengine helpers
#include "kengine/helpers/log_helper.hpp"

namespace kengine {
	bool is_running(const entt::registry & r) noexcept {
		kengine_log(r, very_verbose, "is_running", "Checking for data::keep_alive");
		return !r.view<data::keep_alive>().empty();
	}

	void stop_running(entt::registry & r) noexcept {
		kengine_log(r, verbose, "stop_running", "Removing all data::keep_alive");
		r.clear<data::keep_alive>();
	}
}