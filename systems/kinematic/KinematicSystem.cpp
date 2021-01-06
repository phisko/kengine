#include "KinematicSystem.hpp"
#include "kengine.hpp"

#include "data/KinematicComponent.hpp"
#include "data/PhysicsComponent.hpp"
#include "data/TransformComponent.hpp"

#include "functions/Execute.hpp"

#include "angle.hpp"

namespace kengine::kinematic {
	struct impl {
		static void init(Entity & e) noexcept {
			e += functions::Execute{ execute };
		}

		static void execute(float deltaTime) noexcept {
			for (const auto & [e, transform, physics, kinematic] : entities.with<TransformComponent, PhysicsComponent, KinematicComponent>()) {
				transform.boundingBox.position += physics.movement * deltaTime;

				const auto applyRotation = [deltaTime](float & transformMember, float physicsMember) noexcept {
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
	EntityCreator * KinematicSystem() noexcept {
		return [](Entity & e) noexcept {
			kinematic::impl::init(e);
		};
	}

}
