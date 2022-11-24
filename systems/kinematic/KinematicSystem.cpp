#include "KinematicSystem.hpp"

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// putils
#include "angle.hpp"
#include "forward_to.hpp"

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
		entt::registry & r;

		KinematicSystem(entt::handle e) noexcept
			: r(*e.registry())
		{
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, Log, "Init", "KinematicSystem");

			e.emplace<functions::Execute>(putils_forward_to_this(execute));
		}

		void execute(float deltaTime) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, Verbose, "Execute", "KinematicSystem");

			for (const auto & [e, transform, physics] : r.view<TransformComponent, PhysicsComponent, KinematicComponent>().each()) {
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

	void addKinematicSystem(entt::registry & r) noexcept {
		const entt::handle e{ r, r.create() };
		e.emplace<KinematicSystem>(e);
	}
}
