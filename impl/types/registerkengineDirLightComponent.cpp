#include "helpers/registerTypeHelper.hpp"
#include "data/LightComponent.hpp"

namespace kengine::types{
	void registerkengineDirLightComponent() noexcept {
		kengine::registerComponents<kengine::DirLightComponent>();

	}
}