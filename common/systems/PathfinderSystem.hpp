#pragma once

#include "System.hpp"
#include "EntityManager.hpp"
#include "common/components/TransformComponent.hpp"
#include "common/components/PathfinderComponent.hpp"
#include "common/components/PhysicsComponent.hpp"
#include "common/systems/PhysicsSystem.hpp"
#include "QuadTree.hpp"
#include "AStar.hpp"

namespace kengine {
    class PathfinderSystem : public kengine::System<PathfinderSystem> {
    public:
        PathfinderSystem(kengine::EntityManager & em)
                : putils::BaseModule(&em), _em(em),
                  _tree(query<decltype(_tree)>(packets::QuadTreeQuery{})) {}

    public:
        void execute() noexcept final {
            for (const auto go : _em.getGameObjects<kengine::PathfinderComponent>()) {
                auto & comp = go->getComponent<kengine::PathfinderComponent>();
                if (comp.reached)
                    continue;

                moveTowards(*go, comp);

                if (reached(*go, comp.dest, comp.desiredDistance)) {
                    comp.reached = true;
                    go->getComponent<kengine::PhysicsComponent>().movement = { 0, 0, 0 };
                }
            }
        }

        bool reached(const kengine::GameObject & go, const putils::Point3d & dest, double desiredDistance) {
            const auto & boundingBox = go.getComponent<kengine::TransformComponent3d>().boundingBox;
            return boundingBox.topLeft.distanceTo(dest) <= desiredDistance;
        }

        void moveTowards(kengine::GameObject & go, const PathfinderComponent & comp) {
            auto & phys = go.getComponent<kengine::PhysicsComponent>();
            const auto & box = go.getComponent<kengine::TransformComponent3d>().boundingBox;

            const putils::Point2d start { box.topLeft.x, box.topLeft.z };
            const putils::Point2d end { comp.dest.x, comp.dest.z };

            const auto steps = putils::AStar::getNextDirection<double>(
                    start, end, comp.diagonals,
                    phys.speed * time.getDeltaFrames(), comp.desiredDistance,
                    // phys.speed, comp.desiredDistance,
                    [this, &go, &box, &comp](const putils::Point2d &, const putils::Point2d & to) {
                        return canMoveTo(go, box, to, comp.dest, comp.maxAvoidance);
                    }
            );

            if (steps.empty())
                noPathFound(phys);
            else
                nextStep(steps[0], phys, box.topLeft);
        }

        void noPathFound(kengine::PhysicsComponent & phys) noexcept {
            phys.movement.x = 0;
            phys.movement.z = 0;
        }

        void nextStep(const putils::Point2d & step, kengine::PhysicsComponent & phys, const putils::Point3d & pos) {
            phys.movement.x = putils::sign(step.x - pos.x);
            phys.movement.z = putils::sign(step.y - pos.z);
        }

        bool canMoveTo(const kengine::GameObject & go, const putils::Rect3d & boundingBox,
                       const putils::Point3d & dest, const putils::Point3d & goal, double maxAvoidance) noexcept {
            if (dest.distanceTo(goal) > boundingBox.topLeft.distanceTo(goal) + maxAvoidance)
                return false;

            if (_tree == nullptr)
                return true;

            const auto objects = _tree->query({ { dest.x,             dest.z },
                                                { boundingBox.size.x, boundingBox.size.z } });
            return !anObjectIntersects(objects, go);
        }

        bool anObjectIntersects(const std::vector<kengine::GameObject *> & objects, const kengine::GameObject & go) noexcept {
            const auto & boundingBox = go.getComponent<kengine::TransformComponent3d>().boundingBox;

            return std::any_of(
                    objects.begin(), objects.end(),
                    [&go, &boundingBox](kengine::GameObject * other) {
                        const auto & otherBox = other->getComponent<kengine::TransformComponent3d>().boundingBox;
                        return &go != other && otherBox.intersect(boundingBox);
                    }
            );
        }

    private:
        kengine::EntityManager & _em;
        putils::QuadTree<kengine::GameObject *, double> * _tree;
    };
}