#include "isRunning.hpp"

// entt
#include <entt/entity/registry.hpp>

// kengine data
#include "data/KeepAlive.hpp"

namespace kengine {
	bool isRunning(const entt::registry & r) noexcept {
		return !r.view<KeepAlive>().empty();
	}

	void stopRunning(entt::registry & r) noexcept {
		r.clear<KeepAlive>();
	}
}