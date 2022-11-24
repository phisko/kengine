#include "CollisionSystem.hpp"

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// putils
#include "forward_to.hpp"

// kengine data
#include "data/CollisionComponent.hpp"

// kengine functions
#include "functions/OnCollision.hpp"

// kengine helpers
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine {
	struct CollisionSystem {
		entt::registry & r;

		CollisionSystem(entt::handle e) noexcept
			: r(*e.registry())
		{
			KENGINE_PROFILING_SCOPE;
			e.emplace<functions::OnCollision>(putils_forward_to_this(onCollision));
		}

		void onCollision(entt::entity first, entt::entity second) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_logf(r, Verbose, "CollisionSystem", "Collision between %zu and %zu", first, second);
			trigger(first, second);
			trigger(second, first);
		}

		void trigger(entt::entity first, entt::entity second) noexcept {
			KENGINE_PROFILING_SCOPE;
			const auto collision = r.try_get<CollisionComponent>(first);
			if (!collision || collision->onCollide == nullptr)
				return;
			collision->onCollide(first, second);
		}
	};

	void addCollisionSystem(entt::registry & r) noexcept {
		const entt::handle e{ r, r.create() };
		e.emplace<CollisionSystem>(e);
	}
}
