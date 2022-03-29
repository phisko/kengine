#include "helpers/registerTypeHelper.hpp"
#include "data/LuaTableComponent.hpp"
#include "helpers/logHelper.hpp"

namespace kengine::types{
	void registerkengineLuaTableComponent() noexcept {
		kengine_log(Log, "Init/registerTypes", "Registering 'kengine::LuaTableComponent'");
		kengine::registerComponents<kengine::LuaTableComponent>();

	}
}