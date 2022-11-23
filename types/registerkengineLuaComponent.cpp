#include "helpers/registerTypeHelper.hpp"
#include "data/LuaComponent.hpp"

// entt
#include <entt/entity/fwd.hpp>

// kengine helpers
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine::types{
	void registerkengineLuaComponent(entt::registry & r) noexcept {
#ifdef KENGINE_LUA
		KENGINE_PROFILING_SCOPE;
		kengine_log(r, Log, "Init/registerTypes", "Registering 'kengine::LuaComponent'");
		kengine::registerComponents<kengine::LuaComponent>(r);
#else
		kengine_log(r, Log, "Init/registerTypes", "Not registering 'kengine::LuaComponent' because 'KENGINE_LUA' is not defined");
#endif
	}
}