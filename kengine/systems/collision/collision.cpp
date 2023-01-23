#include "collision.hpp"

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// putils
#include "putils/forward_to.hpp"

// kengine data
#include "kengine/data/collision.hpp"

// kengine functions
#include "kengine/functions/on_collision.hpp"

// kengine helpers
#include "kengine/helpers/log_helper.hpp"
#include "kengine/helpers/profiling_helper.hpp"

namespace kengine::systems {
	struct collision {
		entt::registry & r;

		collision(entt::handle e) noexcept
			: r(*e.registry()) {
			KENGINE_PROFILING_SCOPE;
			e.emplace<functions::on_collision>(putils_forward_to_this(on_collision));
		}

		void on_collision(entt::entity first, entt::entity second) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_logf(r, verbose, "systems/collision", "Collision between %zu and %zu", first, second);
			trigger(first, second);
			trigger(second, first);
		}

		void trigger(entt::entity first, entt::entity second) noexcept {
			KENGINE_PROFILING_SCOPE;
			const auto collision = r.try_get<data::collision>(first);
			if (!collision || collision->on_collide == nullptr)
				return;
			collision->on_collide(first, second);
		}
	};

	entt::entity add_collision(entt::registry & r) noexcept {
		const entt::handle e{ r, r.create() };
		e.emplace<collision>(e);
		return e;
	}
}
