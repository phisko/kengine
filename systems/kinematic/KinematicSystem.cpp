#include "KinematicSystem.hpp"
#include "EntityManager.hpp"

#include "data/KinematicComponent.hpp"
#include "data/PhysicsComponent.hpp"
#include "data/TransformComponent.hpp"

#include "functions/Execute.hpp"

#include "angle.hpp"

namespace kengine::kinematic {
	struct impl {
		static inline EntityManager * em;

		static void init(Entity & e) {
			e += functions::Execute{ execute };
		}

		static void execute(float deltaTime) {
			for (const auto & [e, transform, physics, kinematic] : em->getEntities<TransformComponent, PhysicsComponent, KinematicComponent>()) {
				transform.boundingBox.position += physics.movement * deltaTime;

				const auto applyRotation = [deltaTime](float & transformMember, float physicsMember) {
					transformMember += physicsMember * deltaTime;
					transformMember = putils::constrainAngle(transformMember);
				};

				applyRotation(transform.pitch, physics.pitch);
				applyRotation(transform.yaw, physics.yaw);
				applyRotation(transform.roll, physics.roll);
			}
		}
	};
}

namespace kengine {
	EntityCreatorFunctor<64> KinematicSystem(EntityManager & em) {
		kinematic::impl::em = &em;
		return [&](Entity & e) {
			kinematic::impl::init(e);
		};
	}

}
