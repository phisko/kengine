#include "helpers/registerTypeHelper.hpp"
#include "data/LightComponent.hpp"

namespace kengine::impl::types{
	void registerkengineDirLightComponent() noexcept {
		kengine::registerComponents<kengine::DirLightComponent>();

	}
}