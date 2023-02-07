#include "kinematic.hpp"

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// putils
#include "putils/angle.hpp"
#include "putils/forward_to.hpp"

// kengine data
#include "kengine/data/kinematic.hpp"
#include "kengine/data/physics.hpp"
#include "kengine/data/transform.hpp"

// kengine functions
#include "kengine/functions/execute.hpp"

// kengine helpers
#include "kengine/helpers/log_helper.hpp"
#include "kengine/helpers/profiling_helper.hpp"

namespace kengine::systems {
	struct kinematic {
		entt::registry & r;

		kinematic(entt::handle e) noexcept
			: r(*e.registry()) {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, log, "kinematic", "Initializing");

			e.emplace<functions::execute>(putils_forward_to_this(execute));
		}

		void execute(float delta_time) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, very_verbose, "kinematic", "Executing");

			for (const auto & [e, transform, physics] : r.view<data::transform, data::physics, data::kinematic>().each()) {
				kengine_logf(r, very_verbose, "kinematic", "Moving [%zu]", e);

				transform.bounding_box.position += physics.movement * delta_time;

				const auto apply_rotation = [delta_time](float & transform_member, float physics_member) noexcept {
					transform_member += physics_member * delta_time;
					transform_member = putils::constrain_angle(transform_member);
				};

				apply_rotation(transform.pitch, physics.pitch);
				apply_rotation(transform.yaw, physics.yaw);
				apply_rotation(transform.roll, physics.roll);
			}
		}
	};

	DEFINE_KENGINE_SYSTEM_CREATOR(kinematic)
}
