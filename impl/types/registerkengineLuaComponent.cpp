#include "helpers/registerTypeHelper.hpp"
#include "data/LuaComponent.hpp"

namespace kengine::types{
	void registerkengineLuaComponent() noexcept {
		kengine::registerComponents<kengine::LuaComponent>();

	}
}