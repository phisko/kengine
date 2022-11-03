#include "helpers/registerTypeHelper.hpp"
#include "data/PhysicsComponent.hpp"
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine::types{
	void registerkenginePhysicsComponent() noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(Log, "Init/registerTypes", "Registering 'kengine::PhysicsComponent'");
		kengine::registerComponents<kengine::PhysicsComponent>();
	}
}