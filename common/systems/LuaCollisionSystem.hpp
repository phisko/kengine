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
                  _lua(*query<kengine::packets::LuaState::Response>(kengine::packets::LuaState::Query{}).state) {
            _lua["collisions"] = sol::new_table();
            _lua["collisionHandlers"] = sol::new_table();
        }

    public:
        void handle(const kengine::packets::Collision & p) noexcept {
            const auto & firstName = p.first.getName();
            const auto & secondName = p.second.getName();

            _lua["collisions"][firstName] = secondName;
            _lua["collisions"][secondName] = firstName;

            sol::table handlers = _lua["collisionHandlers"];
            handlers.for_each([&firstName, &secondName](auto && pair) {
                sol::function f = pair.second;
                f(firstName, secondName);
            });
        }

    private:
        sol::state & _lua;
    };
}
