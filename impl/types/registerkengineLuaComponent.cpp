#include "helpers/registerTypeHelper.hpp"
#include "data/LuaComponent.hpp"

namespace kengine::impl::types{
	void registerkengineLuaComponent() noexcept {
		kengine::registerComponents<kengine::LuaComponent>();

	}
}