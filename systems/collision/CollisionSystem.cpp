#include "CollisionSystem.hpp"
#include "kengine.hpp"

#include "functions/OnCollision.hpp"
#include "data/CollisionComponent.hpp"

namespace kengine::collision {
	struct impl {
		static void init(Entity & e) noexcept {
			e += functions::OnCollision{ onCollision };
		}

		static void onCollision(Entity & first, Entity & second) noexcept {
			trigger(first, second);
			trigger(second, first);
		}

		static void trigger(Entity & first, Entity & second) noexcept {
			const auto collision = first.tryGet<CollisionComponent>();
			if (!collision || collision->onCollide == nullptr)
				return;
			collision->onCollide(first, second);
		}
	};
}

namespace kengine {
	EntityCreator * CollisionSystem() noexcept {
		return [](Entity & e) noexcept {
			collision::impl::init(e);
		};
	}
}
