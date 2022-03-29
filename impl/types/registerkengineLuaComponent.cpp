#include "helpers/registerTypeHelper.hpp"
#include "data/LuaComponent.hpp"
#include "helpers/logHelper.hpp"

namespace kengine::types{
	void registerkengineLuaComponent() noexcept {
		kengine_log(Log, "Init/registerTypes", "Registering 'kengine::LuaComponent'");
		kengine::registerComponents<kengine::LuaComponent>();

	}
}