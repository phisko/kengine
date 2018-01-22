#pragma once

#include "EntityManager.hpp"
#include "System.hpp"
#include "common/components/PhysicsComponent.hpp"
#include "common/components/TransformComponent.hpp"
#include "common/packets/Position.hpp"
#include "common/packets/Collision.hpp"
#include "QuadTree.hpp"

namespace kengine {
    namespace packets {
        struct QuadTreeQuery {
            putils::BaseModule * sender;
        };
    }

    class PhysicsSystem : public kengine::System<PhysicsSystem,
            kengine::packets::RegisterGameObject, kengine::packets::RemoveGameObject,
            packets::Position::Query, packets::QuadTreeQuery> {
    public:
        PhysicsSystem(kengine::EntityManager & em)
                : _tree({ { 0,  0 },
                          { 64, 64 } }),
                  _em(em) {}

    public:
        void execute() final {
            for (const auto go : _em.getGameObjects<PhysicsComponent>())
                updatePosition(*go);
        }

        void handle(const kengine::packets::RegisterGameObject & p) noexcept {
            auto & go = p.go;
            if (go.hasComponent<kengine::TransformComponent3d>() && go.hasComponent<kengine::PhysicsComponent>() &&
                go.getComponent<kengine::PhysicsComponent>().solid) {
                const auto & box = go.getComponent<kengine::TransformComponent3d>().boundingBox;
                _tree.add(&go, { { box.topLeft.x, box.topLeft.z },
                                 { box.size.x,    box.size.z } });
            }
        }

        void handle(const kengine::packets::RemoveGameObject & p) noexcept {
            auto & go = p.go;
            if (go.hasComponent<kengine::TransformComponent3d>() && go.hasComponent<kengine::PhysicsComponent>() &&
                go.getComponent<kengine::PhysicsComponent>().solid)
                _tree.remove(&go);
        }

    public:
        void handle(const packets::Position::Query & q) {
            sendTo(
                    packets::Position::Response {
                            _tree.query(
                                    {
                                            { q.box.topLeft.x, q.box.topLeft.z },
                                            { q.box.size.x,    q.box.size.z }
                                    }
                            )
                    },
                    *q.sender
            );
        }

        void handle(const packets::QuadTreeQuery & q) {
            sendTo(&_tree, *q.sender);
        }

        // Helpers
    private:
        void updatePosition(kengine::GameObject & go) {
            const auto & phys = go.getComponent<PhysicsComponent>();
            if (phys.fixed)
                return;

            auto & box = go.getComponent<kengine::TransformComponent3d>().boundingBox;

            const auto dest = getNewPos(box.topLeft, phys.movement, phys.speed);
            if (dest == box.topLeft)
                return;

            const auto pos = putils::Rect2d{ { dest.x,     dest.z },
                                             { box.size.x, box.size.z } };
            box.topLeft = dest;

            if (phys.solid) {
                _tree.move(&go, pos);
                checkCollisions(go, pos);
            }
        }

        putils::Point3d getNewPos(const putils::Point3d & pos, const putils::Point3d & movement, double speed) {
            return putils::Point3d {
                    pos.x + movement.x * time.getDeltaFrames() * speed,
                    pos.y + movement.y * time.getDeltaFrames() * speed,
                    pos.z + movement.z * time.getDeltaFrames() * speed,
            };
        }

        void checkCollisions(kengine::GameObject & go, const putils::Rect2d & box) {
            const auto objects = _tree.query(box);

            for (const auto obj : objects)
                if (obj != &go)
                    send(kengine::packets::Collision{ go, *obj });
        }

    private:
        kengine::EntityManager & _em;
        putils::QuadTree<kengine::GameObject *, double> _tree;
    };
}
