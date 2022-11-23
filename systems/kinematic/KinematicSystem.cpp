#include "KinematicSystem.hpp"

// entt
#include <entt/entity/registry.hpp>

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
#include "helpers/profilingHelper.hpp"

namespace kengine {
	struct KinematicSystem {
		static void init(entt::registry & r) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, Log, "Init", "KinematicSystem");

			_r = &r;

			const auto e = r.create();
			r.emplace<functions::Execute>(e, execute);
		}

		static void execute(float deltaTime) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(*_r, Verbose, "Execute", "KinematicSystem");

			for (const auto & [e, transform, physics] : _r->view<TransformComponent, PhysicsComponent, KinematicComponent>().each()) {
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

		static inline entt::registry * _r;
	};

	void KinematicSystem(entt::registry & r) noexcept {
		KinematicSystem::init(r);
	}
}
