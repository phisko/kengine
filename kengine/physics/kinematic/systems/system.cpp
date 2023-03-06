#include "system.hpp"

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// putils
#include "putils/angle.hpp"
#include "putils/forward_to.hpp"

// kengine
#include "kengine/core/data/transform.hpp"
#include "kengine/core/log/helpers/kengine_log.hpp"
#include "kengine/core/profiling/helpers/kengine_profiling_scope.hpp"
#include "kengine/main_loop/functions/execute.hpp"
#include "kengine/physics/data/inertia.hpp"
#include "kengine/physics/kinematic/data/kinematic.hpp"

namespace kengine::physics::kinematic {
	static constexpr auto log_category = "physics_kinematic";

	struct system {
		entt::registry & r;

		system(entt::handle e) noexcept
			: r(*e.registry()) {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, log, log_category, "Initializing");

			e.emplace<main_loop::execute>(putils_forward_to_this(execute));
		}

		void execute(float delta_time) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, very_verbose, log_category, "Executing");

			for (const auto & [e, transform, inertia] : r.view<core::transform, inertia, kinematic>().each()) {
				kengine_logf(r, very_verbose, log_category, "Moving {}", e);

				transform.bounding_box.position += inertia.movement * delta_time;

				const auto apply_rotation = [delta_time](float & transform_member, float physics_member) noexcept {
					transform_member += physics_member * delta_time;
					transform_member = putils::constrain_angle(transform_member);
				};

				apply_rotation(transform.pitch, inertia.pitch);
				apply_rotation(transform.yaw, inertia.yaw);
				apply_rotation(transform.roll, inertia.roll);
			}
		}
	};

	DEFINE_KENGINE_SYSTEM_CREATOR(system)
}
