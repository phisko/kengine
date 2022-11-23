#include "helpers/registerTypeHelper.hpp"
#include "data/LuaTableComponent.hpp"

// entt
#include <entt/entity/fwd.hpp>

// kengine helpers
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine::types{
	void registerkengineLuaTableComponent(entt::registry & r) noexcept {
#ifdef KENGINE_LUA
		KENGINE_PROFILING_SCOPE;
		kengine_log(r, Log, "Init/registerTypes", "Registering 'kengine::LuaTableComponent'");
		kengine::registerComponents<kengine::LuaTableComponent>(r);
#else
		kengine_log(r, Log, "Init/registerTypes", "Not registering 'kengine::LuaTableComponent' because 'KENGINE_LUA' is not defined");
#endif
	}
}