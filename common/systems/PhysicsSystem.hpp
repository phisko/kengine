#pragma once

#include "EntityManager.hpp"
#include "System.hpp"
#include "common/components/PhysicsComponent.hpp"
#include "common/components/TransformComponent.hpp"
#include "common/packets/Position.hpp"
#include "common/packets/Collision.hpp"

namespace kengine {
    class PhysicsSystem : public kengine::System<PhysicsSystem, packets::Position::Query> {
    public:
        PhysicsSystem(kengine::EntityManager & em) : System(em), _em(em) {}

    public:
        void execute() final {
            for (auto & [e, phys] : _em.getEntities<PhysicsComponent>())
                updatePosition(e, phys);
        }

    public:
        void handle(const packets::Position::Query & q) {
            std::vector<Entity *> found;

            for (const auto & [e, phys, transform] : _em.getEntities<PhysicsComponent, TransformComponent3f>()) {
                auto & box = transform.boundingBox;
                if (box.intersect(q.box))
                    found.push_back(&e);
            }

			sendTo(packets::Position::Response{ found }, *q.sender);
		}

		// Helpers
	private:
		void updatePosition(Entity & e, const PhysicsComponent & phys) {
			if (phys.fixed)
				return;

			auto & box = e.get<TransformComponent3f>().boundingBox;

			const auto dest = getNewPos(box.topLeft, phys.movement, phys.speed);
			if (dest == box.topLeft)
				return;
			box.topLeft = dest;

			if (phys.solid)
				checkCollisions(e, box);
		}

		putils::Point3f getNewPos(const putils::Point3f & pos, const putils::Vector3f & movement, float speed) {
			return {
					pos.x + movement.x * time.getDeltaFrames() * speed,
					pos.y + movement.y * time.getDeltaFrames() * speed,
					pos.z + movement.z * time.getDeltaFrames() * speed,
			};
		}

		void checkCollisions(Entity & go, const putils::Rect3f & box) {
			for (const auto & [obj, phys, transform] : _em.getEntities<PhysicsComponent, TransformComponent3f>()) {
				if (&obj == &go)
					continue;

				const auto & other = transform.boundingBox;
				if (box.intersect(other))
					send(packets::Collision{ go, obj });
			}
		}

	private:
		EntityManager & _em;
	};
}
