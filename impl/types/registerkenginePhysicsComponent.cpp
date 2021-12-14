#include "helpers/registerTypeHelper.hpp"
#include "data/PhysicsComponent.hpp"

namespace kengine::impl::types{
	void registerkenginePhysicsComponent() noexcept {
		kengine::registerComponents<kengine::PhysicsComponent>();

	}
}