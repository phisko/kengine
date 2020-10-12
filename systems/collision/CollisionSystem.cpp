#include "CollisionSystem.hpp"
#include "EntityManager.hpp"

#include "functions/OnCollision.hpp"
#include "data/CollisionComponent.hpp"

namespace kengine::collision {
	struct impl {
		static void init(Entity & e) {
			e += functions::OnCollision{ onCollision };
		}

		static void onCollision(Entity & first, Entity & second) {
			trigger(first, second);
			trigger(second, first);
		}

		static void trigger(Entity & first, Entity & second) {
			const auto collision = first.tryGet<CollisionComponent>();
			if (!collision || collision->onCollide == nullptr)
				return;
			collision->onCollide(first, second);
		}
	};
}

namespace kengine {
	EntityCreator * CollisionSystem(EntityManager & em) {
		return [](Entity & e) {
			collision::impl::init(e);
		};
	}
}
