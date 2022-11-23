#include "CollisionSystem.hpp"

// entt
#include <entt/entity/registry.hpp>

// kengine data
#include "data/CollisionComponent.hpp"

// kengine functions
#include "functions/OnCollision.hpp"

// kengine helpers
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine {
	struct CollisionSystem {
		static void init(entt::registry & r) noexcept {
			KENGINE_PROFILING_SCOPE;

			_r = &r;

			const auto e = r.create();
			r.emplace<functions::OnCollision>(e, onCollision);
		}

		static void onCollision(entt::entity first, entt::entity second) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_logf(*_r, Verbose, "CollisionSystem", "Collision between %zu and %zu", first, second);
			trigger(first, second);
			trigger(second, first);
		}

		static void trigger(entt::entity first, entt::entity second) noexcept {
			KENGINE_PROFILING_SCOPE;
			const auto collision = _r->try_get<CollisionComponent>(first);
			if (!collision || collision->onCollide == nullptr)
				return;
			collision->onCollide(first, second);
		}

		static inline entt::registry * _r;
	};

	void CollisionSystem(entt::registry & r) noexcept {
		CollisionSystem::init(r);
	}
}
