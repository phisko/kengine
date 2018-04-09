#pragma once

#include "EntityManager.hpp"
#include "System.hpp"
#include "common/packets/Position.hpp"
#include "Box2D/Box2D.hpp"

namespace kengine {
    class Box2DSystem : public kengine::System<Box2DSystem,
            kengine::packets::RegisterGameObject, kengine::packets::RemoveGameObject,
            packets::Position::Query> {
    public:
        Box2DSystem(kengine::EntityManager & em);

    public:
        void execute() noexcept final;
        void handle(const kengine::packets::RegisterGameObject & p) noexcept;
        void handle(const kengine::packets::RemoveGameObject & p) noexcept;

    public:
        void handle(const packets::Position::Query & q) noexcept;

        // Helpers
    private:
        void updateBody(kengine::GameObject & go) noexcept;
        void updateTransform(kengine::GameObject & go) noexcept;
        void handleCollisions() noexcept;

    private:
        kengine::EntityManager & _em;
        b2::World _world{ { 0, 0 } };
    };
}
