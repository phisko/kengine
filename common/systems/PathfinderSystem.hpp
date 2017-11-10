#pragma once

#include "System.hpp"
#include "EntityManager.hpp"
#include "common/components/TransformComponent.hpp"
#include "common/components/PathfinderComponent.hpp"
#include "common/components/PhysicsComponent.hpp"
#include "QuadTree.hpp"
#include "AStar.hpp"

namespace kengine
{
    class PathfinderSystem : public kengine::System<PathfinderSystem>
    {
    public:
        PathfinderSystem(kengine::EntityManager &em)
                : putils::BaseModule(&em), _em(em),
                  _tree(query<decltype(_tree)>(packets::QuadTreeQuery{}))
        {}

    public:
        void execute() noexcept final
        {
            for (const auto go : _em.getGameObjects<kengine::PathfinderComponent>())
            {
                auto &comp = go->getComponent<kengine::PathfinderComponent>();
                if (comp.reached)
                    continue;

                moveTowards(*go, comp);
                const auto &boundingBox = go->getComponent<kengine::TransformComponent3d>().boundingBox;
                if (boundingBox.topLeft.distanceTo(comp.dest) <= comp.desiredDistance)
                {
                    comp.reached = true;
                    go->getComponent<kengine::PhysicsComponent>().movement = { 0, 0, 0 };
                }
            }
        }

        void moveTowards(kengine::GameObject &go, const PathfinderComponent &comp)
        {
            auto &phys = go.getComponent<kengine::PhysicsComponent>();
            const auto &box = go.getComponent<kengine::TransformComponent3d>().boundingBox;

            const putils::Point2d start{ box.topLeft.x, box.topLeft.z };
            const putils::Point2d end{ comp.dest.x, comp.dest.z };

            const auto steps = putils::AStar::getNextDirection<double>(
                    start, end, comp.diagonals,
                    // phys.speed * time.getDeltaFrames(), comp.desiredDistance,
                    phys.speed, comp.desiredDistance,
                    [this, &go, &box, &comp](const putils::Point2d &, const putils::Point2d &to)
                    {
                        return canMoveTo(go, box, to, comp.dest, comp.maxAvoidance);
                    }
            );

            if (steps.empty())
            {
                phys.movement.x = 0;
                phys.movement.z = 0;
            }
            else
            {
                const auto &next = steps[0];
                phys.movement.x = putils::sign(next.x - box.topLeft.x);
                phys.movement.z = putils::sign(next.y - box.topLeft.z);
            }
        }

        bool canMoveTo(const kengine::GameObject &go, const putils::Rect3d &boundingBox,
                       const putils::Point3d &dest, const putils::Point3d &goal, double maxAvoidance)
        {
            if (dest.distanceTo(goal) > boundingBox.topLeft.distanceTo(goal) + maxAvoidance)
                return false;

            const auto objects = _tree->query({ { dest.x, dest.z }, { boundingBox.size.x, boundingBox.size.z } });
            for (const auto other : objects)
                if (&go != other)
                {
                    std::cout << "Evaluating " << other->getName() << std::endl;
                    const auto &otherBox = other->getComponent<kengine::TransformComponent3d>().boundingBox;
                    if (boundingBox.intersect(otherBox))
                        return false;
                }
            return true;
        }

    private:
        kengine::EntityManager &_em;
        putils::QuadTree<kengine::GameObject *, double> *_tree;
    };
}