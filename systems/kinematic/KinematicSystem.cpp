#include "KinematicSystem.hpp"
#include "kengine.hpp"

// putils
#include "angle.hpp"

// kengine data
#include "data/KinematicComponent.hpp"
#include "data/PhysicsComponent.hpp"
#include "data/TransformComponent.hpp"

// kengine functions
#include "functions/Execute.hpp"

// kengine helpers
#include "helpers/logHelper.hpp"

namespace kengine::kinematic {
	struct impl {
		static void init(Entity & e) noexcept {
			kengine_log(Log, "Init", "KinematicSystem");
			e += functions::Execute{ execute };
		}

		static void execute(float deltaTime) noexcept {
			kengine_log(Verbose, "Execute", "KinematicSystem");
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
