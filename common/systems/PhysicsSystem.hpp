#pragma once

#include "EntityManager.hpp"
#include "System.hpp"
#include "common/components/PhysicsComponent.hpp"
#include "common/components/TransformComponent.hpp"
#include "common/packets/Position.hpp"
#include "QuadTree.hpp"

namespace kengine
{
   namespace packets
   {
       struct QuadTreeQuery
       {
           putils::BaseModule *sender;
       };
   }

    class PhysicsSystem : public kengine::System<PhysicsSystem, packets::Position::Query, packets::QuadTreeQuery>
    {
    public:
        PhysicsSystem(kengine::EntityManager &em)
                : _tree({ { 0, 0 }, { 64, 64 } }),
                  _em(em)
        {}

    public:
        void execute() final
        {
            for (const auto go : _em.getGameObjects<PhysicsComponent>())
                updatePosition(*go);
        }

        void registerGameObject(kengine::GameObject &go) noexcept final
        {
            if (go.hasComponent<kengine::TransformComponent3d>()
                && go.hasComponent<kengine::PhysicsComponent>()
                && go.getComponent<kengine::PhysicsComponent>().solid)
            {
                const auto &box = go.getComponent<kengine::TransformComponent3d>().boundingBox;
                _tree.add(&go, { { box.topLeft.x, box.topLeft.z },
                                 { box.size.x, box.size.z } });
            }
        }

    public:
        void handle(const packets::Position::Query &q)
        {
            const auto objects = _tree.query(
                    {
                            { q.box.topLeft.x, q.box.topLeft.z },
                            { q.box.size.x, q.box.size.z }
                    }
            );

            sendTo(packets::Position::Response{std::move(objects)}, *q.sender);
        }

        void handle(const packets::QuadTreeQuery &q)
        {
            sendTo(&_tree, *q.sender);
        }

        // Helpers
    private:
        void updatePosition(kengine::GameObject &go)
        {
            const auto &phys = go.getComponent<PhysicsComponent>();
            if (phys.fixed)
                return;

            auto &box = go.getComponent<kengine::TransformComponent3d>().boundingBox;

            const auto dest = getNewPos(box.topLeft, phys.movement, phys.speed);
            if (dest == box.topLeft)
                return;

            const auto pos = putils::Rect2d{ { dest.x, dest.z }, { box.size.x, box.size.z } };
            if (canMoveTo(go, pos))
            {
                box.topLeft = dest; // No collision
                _tree.move(&go, pos);
            }
        }

        putils::Point3d getNewPos(const putils::Point3d &pos, const putils::Point3d &movement, double speed)
        {
            auto ret = pos;
            ret.x += movement.x * time.getDeltaFrames() * speed;
            ret.y += movement.y * time.getDeltaFrames() * speed;
            ret.z += movement.z * time.getDeltaFrames() * speed;
            return ret;
        }

        bool canMoveTo(kengine::GameObject &go, const putils::Rect2d &pos)
        {
            const auto objects =_tree.query(pos);
            for (const auto obj : objects)
                if (obj != &go)
                {
                    const auto &me = go.getComponent<kengine::TransformComponent3d>().boundingBox;
                    const auto &other = obj->getComponent<kengine::TransformComponent3d>().boundingBox;
                    return false;
                }
            return true;
        }

    private:
        kengine::EntityManager &_em;
        putils::QuadTree<kengine::GameObject *, double> _tree;
    };
}
