#include "helpers/registerTypeHelper.hpp"
#include "data/PhysicsComponent.hpp"

// entt
#include <entt/entity/fwd.hpp>

// kengine helpers
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine::types{
	void registerkenginePhysicsComponent(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(r, Log, "Init/registerTypes", "Registering 'kengine::PhysicsComponent'");
		kengine::registerComponents<kengine::PhysicsComponent>(r);
	}
}