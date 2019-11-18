#pragma once

#include "ScriptSystem.hpp"

#include "lua/plua.hpp"

#include "EntityManager.hpp"

#include "common/components/LuaComponent.hpp"
#include "common/packets/LuaState.hpp"

namespace kengine {
    class LuaSystem : public ScriptSystem<LuaSystem, LuaComponent, packets::LuaState::Query> {
    public:
        LuaSystem(EntityManager & em) : ScriptSystem(em) {
            _lua.open_libraries();
			ScriptSystem::init();
        }

    public:
		template<typename Ret, typename ...Args>
		void registerFunction(const std::string & name, const std::function<Ret(Args...)> & func) {
			_lua[name] = FWD(func);
		}

		template<typename Ret, typename ...Args>
		void registerEntityMember(const std::string & name, const std::function<Ret(Args...)> & func) {
			_lua[putils::reflection::get_class_name<Entity>()][name] = FWD(func);
		}

		template<typename T>
		void registerTypeInternal() {
			putils::lua::registerType<T>(_lua);
		}

    public:
        void executeScript(const char * fileName) noexcept {
			_lua.script_file(fileName);
        }

    public:
		void setSelf(Entity & go) { _lua["self"] = &go; }
		void unsetSelf() const { _lua["self"] = nullptr; }

    public:
		sol::state & getState() { return _lua;  }

    public:
        void handle(const packets::LuaState::Query & q) noexcept {
            sendTo(packets::LuaState::Response{ &_lua }, *q.sender);
        }

    private:
        sol::state & _lua = *(new sol::state);
    };
}