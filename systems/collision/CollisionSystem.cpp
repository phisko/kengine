#include "CollisionSystem.hpp"
#include "kengine.hpp"

// kengine data
#include "data/CollisionComponent.hpp"

// kengine functions
#include "functions/OnCollision.hpp"

// kengine helpers
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine {
	struct CollisionSystem {
		static void init(Entity & e) noexcept {
			KENGINE_PROFILING_SCOPE;
			e += functions::OnCollision{ onCollision };
		}

		static void onCollision(Entity & first, Entity & second) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_logf(Verbose, "CollisionSystem", "Collision between %zu and %zu", first.id, second.id);
			trigger(first, second);
			trigger(second, first);
		}

		static void trigger(Entity & first, Entity & second) noexcept {
			KENGINE_PROFILING_SCOPE;
			const auto collision = first.tryGet<CollisionComponent>();
			if (!collision || collision->onCollide == nullptr)
				return;
			collision->onCollide(first, second);
		}
	};

	EntityCreator * CollisionSystem() noexcept {
		return CollisionSystem::init;
	}
}
