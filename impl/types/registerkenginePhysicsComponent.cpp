#include "helpers/registerTypeHelper.hpp"
#include "data/PhysicsComponent.hpp"
#include "helpers/logHelper.hpp"

namespace kengine::types{
	void registerkenginePhysicsComponent() noexcept {
		kengine_log(Log, "Init/registerTypes", "Registering 'kengine::PhysicsComponent'");
		kengine::registerComponents<kengine::PhysicsComponent>();

	}
}