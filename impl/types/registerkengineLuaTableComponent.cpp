#include "helpers/registerTypeHelper.hpp"
#include "data/LuaTableComponent.hpp"
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine::types{
	void registerkengineLuaTableComponent() noexcept {
#ifdef KENGINE_LUA
		KENGINE_PROFILING_SCOPE;
		kengine_log(Log, "Init/registerTypes", "Registering 'kengine::LuaTableComponent'");
		kengine::registerComponents<kengine::LuaTableComponent>();
#else
		kengine_log(Log, "Init/registerTypes", "Not registering 'kengine::LuaTableComponent' because 'KENGINE_LUA' is not defined");
#endif
	}
}