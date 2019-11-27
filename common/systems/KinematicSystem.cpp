#include "KinematicSystem.hpp"
#include "components/TransformComponent.hpp"
#include "components/PhysicsComponent.hpp"

namespace kengine {
	KinematicSystem::KinematicSystem(EntityManager & em)
		: System(em), _em(em)
	{}

	void KinematicSystem::execute() {
		const auto deltaTime = time.getDeltaTime().count();

		for (const auto & [e, transform, physics] : _em.getEntities<TransformComponent3f, PhysicsComponent>()) {
			if (!physics.kinematic)
				continue;
			transform.boundingBox.position += physics.movement * physics.speed * deltaTime;
			transform.pitch += physics.pitch * physics.speed * deltaTime;
			transform.yaw += physics.yaw * physics.speed * deltaTime;
			transform.roll += physics.roll * physics.speed * deltaTime;
		}
	}
}
