#include "helpers/registerTypeHelper.hpp"
#include "data/LuaComponent.hpp"
#include "helpers/logHelper.hpp"

namespace kengine::types{
	void registerkengineLuaComponent() noexcept {
#ifdef KENGINE_LUA
		kengine_log(Log, "Init/registerTypes", "Registering 'kengine::LuaComponent'");
		kengine::registerComponents<kengine::LuaComponent>();
#else
		kengine_log(Log, "Init/registerTypes", "Not registering 'kengine::LuaComponent' because 'KENGINE_LUA' is not defined");
#endif
	}
}