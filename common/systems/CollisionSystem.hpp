#pragma once

#include "System.hpp"
#include "packets/Collision.hpp"
#include "components/CollisionComponent.hpp"

namespace kengine {
	class CollisionSystem : public System<CollisionSystem, packets::Collision> {
	public:
		CollisionSystem(kengine::EntityManager &) {}

	public:
		void handle(const kengine::packets::Collision & p) {
			trigger(p.first, p.second);
			trigger(p.second, p.first);
		}

	private:
		void trigger(kengine::GameObject & go, kengine::GameObject & other) {
			if (go.hasComponent<CollisionComponent>()) {
				const auto & comp = go.getComponent<CollisionComponent>();
				if (comp.onCollide != nullptr)
					comp.onCollide(go, other);
			}
		}
	};
}