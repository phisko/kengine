#pragma once

#include "EntityManager.hpp"
#include "System.hpp"
#include "common/components/PhysicsComponent.hpp"
#include "common/components/TransformComponent.hpp"
#include "common/packets/Position.hpp"
#include "common/packets/Collision.hpp"
#include "with.hpp"

namespace kengine {
    class PhysicsSystem : public kengine::System<PhysicsSystem, packets::Position::Query> {
    public:
        PhysicsSystem(kengine::EntityManager & em) : _em(em) {}

    public:
        void execute() final {
            for (const auto go : _em.getGameObjects<PhysicsComponent>())
                updatePosition(*go);
        }

    public:
        void handle(const packets::Position::Query & q) {
            std::vector<kengine::GameObject *> found;

            for (const auto go : _em.getGameObjects<kengine::PhysicsComponent>()) {
                auto & box = go->getComponent<kengine::TransformComponent3d>().boundingBox;
                if (box.intersect(q.box))
                    found.push_back(go);
            }

			sendTo(packets::Position::Response{ found }, *q.sender);
		}

		// Helpers
	private:
		void updatePosition(kengine::GameObject & go) {
			{ pmeta_with(go.getComponent<PhysicsComponent>()) {
				if (_.fixed)
					return;

				auto & box = go.getComponent<kengine::TransformComponent3d>().boundingBox;

				const auto dest = getNewPos(box.topLeft, _.movement, _.speed);
				if (dest == box.topLeft)
					return;
				box.topLeft = dest;

				if (_.solid)
					checkCollisions(go, box);
			}}
		}

		putils::Point3d getNewPos(const putils::Point3d & pos, const putils::Point3d & movement, double speed) {
			return putils::Point3d{
					pos.x + movement.x * time.getDeltaFrames() * speed,
					pos.y + movement.y * time.getDeltaFrames() * speed,
					pos.z + movement.z * time.getDeltaFrames() * speed,
			};
		}

		void checkCollisions(kengine::GameObject & go, const putils::Rect3d & box) {
			for (const auto obj : _em.getGameObjects<kengine::PhysicsComponent>()) {
				if (obj == &go)
					continue;

				const auto & other = obj->getComponent<kengine::TransformComponent3d>().boundingBox;
				if (box.intersect(other))
					send(kengine::packets::Collision{ go, *obj });
			}
		}

	private:
		kengine::EntityManager & _em;
	};
}
