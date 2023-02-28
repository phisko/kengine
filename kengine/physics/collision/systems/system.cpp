#include "system.hpp"

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// putils
#include "putils/forward_to.hpp"

// kengine core
#include "kengine/core/log/helpers/kengine_log.hpp"
#include "kengine/core/profiling/helpers/kengine_profiling_scope.hpp"

// kengine physics
#include "kengine/physics/functions/on_collision.hpp"

// kengine physics/collision
#include "kengine/physics/collision/data/collision.hpp"

namespace kengine::physics::collision {
	static constexpr auto log_category = "physics_collision";

	struct system {
		entt::registry & r;

		system(entt::handle e) noexcept
			: r(*e.registry()) {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, log, log_category, "Initializing");
			e.emplace<physics::on_collision>(putils_forward_to_this(on_collision));
		}

		void on_collision(entt::entity first, entt::entity second) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_logf(r, verbose, log_category, "Collision between [%u] and [%u]", first, second);
			trigger(first, second);
			trigger(second, first);
		}

		void trigger(entt::entity first, entt::entity second) noexcept {
			KENGINE_PROFILING_SCOPE;
			const auto collision = r.try_get<physics::collision::collision>(first);
			if (!collision || collision->on_collide == nullptr)
				return;
			collision->on_collide(first, second);
		}
	};

	DEFINE_KENGINE_SYSTEM_CREATOR(system)
}
