#include "helpers/registerTypeHelper.hpp"
#include "data/LuaTableComponent.hpp"

namespace kengine::types{
	void registerkengineLuaTableComponent() noexcept {
		kengine::registerComponents<kengine::LuaTableComponent>();

	}
}