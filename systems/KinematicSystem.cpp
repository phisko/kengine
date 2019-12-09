#include "KinematicSystem.hpp"
#include "EntityManager.hpp"

#include "data/TransformComponent.hpp"
#include "data/PhysicsComponent.hpp"
#include "functions/Execute.hpp"

#include "angle.hpp"

namespace kengine {
	static void execute(EntityManager & em, float deltaTime);
	EntityCreatorFunctor<64> KinematicSystem(EntityManager & em) {
		return [&](Entity & e) {
			e += functions::Execute{ [&](float deltaTime) { execute(em, deltaTime); } };
		};
	}

	static void execute(EntityManager & em, float deltaTime) {
		for (const auto & [e, transform, physics, kinematic] : em.getEntities<TransformComponent3f, PhysicsComponent, KinematicComponent>()) {
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
