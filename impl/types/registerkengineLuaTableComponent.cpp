#include "helpers/registerTypeHelper.hpp"
#include "data/LuaTableComponent.hpp"
#include "helpers/logHelper.hpp"

namespace kengine::types{
	void registerkengineLuaTableComponent() noexcept {
#ifdef KENGINE_LUA
		kengine_log(Log, "Init/registerTypes", "Registering 'kengine::LuaTableComponent'");
		kengine::registerComponents<kengine::LuaTableComponent>();
#else
		kengine_log(Log, "Init/registerTypes", "Not registering 'kengine::LuaTableComponent' because 'KENGINE_LUA' is not defined");
#endif
	}
}