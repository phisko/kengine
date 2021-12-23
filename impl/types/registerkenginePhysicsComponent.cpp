#include "helpers/registerTypeHelper.hpp"
#include "data/PhysicsComponent.hpp"

namespace kengine::types{
	void registerkenginePhysicsComponent() noexcept {
		kengine::registerComponents<kengine::PhysicsComponent>();

	}
}