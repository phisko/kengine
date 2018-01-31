#pragma once

#include "lua/plua.hpp"
#include "EntityManager.hpp"
#include "LuaSystem.hpp"

#include "System.hpp"
#include "common/packets/Collision.hpp"
#include "common/packets/LuaState.hpp"

namespace kengine {
    class EntityManager;

    class LuaCollisionSystem : public kengine::System<LuaCollisionSystem, kengine::packets::Collision> {
    public:
        LuaCollisionSystem(kengine::EntityManager & em)
                : putils::BaseModule(&em),
                  _lua(*query<kengine::packets::LuaState::Response>(kengine::packets::LuaState::Query{}).state)
        {}

    public:
        void handle(const kengine::packets::Collision & p) noexcept {
            callHandler(p.first, p.second);
            callHandler(p.second, p.first);
        }

        void callHandler(kengine::GameObject & go, kengine::GameObject & other) noexcept {
            if (go.hasComponent<kengine::LuaComponent>()) {
                auto & meta = go.getComponent<kengine::LuaComponent>().meta;
                if (meta != sol::nil && meta["onCollision"] != sol::nil) {
                    sol::function handler = meta["onCollision"];
                    handler(go, other);
                }
            }
        }

    private:
        sol::state & _lua;
    };
}
