#include "KinematicSystem.hpp"
#include "components/TransformComponent.hpp"
#include "components/PhysicsComponent.hpp"

#include "angle.hpp"

namespace kengine {
	KinematicSystem::KinematicSystem(EntityManager & em)
		: System(em), _em(em)
	{}

	void KinematicSystem::execute() {
		const auto deltaTime = time.getDeltaTime().count();

		for (const auto & [e, transform, physics, kinematic] : _em.getEntities<TransformComponent3f, PhysicsComponent, KinematicComponent>()) {
			transform.boundingBox.position += physics.movement * physics.speed * deltaTime;

			const auto applyRotation = [&](float & transformMember, float physicsMember) {
				transformMember += physicsMember * physics.speed * deltaTime;
				transformMember = putils::constrainAngle(transformMember);
			};

			applyRotation(transform.pitch, physics.pitch);
			applyRotation(transform.yaw, physics.yaw);
			applyRotation(transform.roll, physics.roll);
		}
	}
}
