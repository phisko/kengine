#include "helpers/registerTypeHelper.hpp"
#include "data/LuaTableComponent.hpp"

namespace kengine::impl::types{
	void registerkengineLuaTableComponent() noexcept {
		kengine::registerComponents<kengine::LuaTableComponent>();

	}
}