#include "helpers/registerTypeHelper.hpp"
#include "data/LightComponent.hpp"
#include "helpers/logHelper.hpp"

namespace kengine::types{
	void registerkengineDirLightComponent() noexcept {
		kengine_log(Log, "Init/registerTypes", "Registering 'kengine::DirLightComponent'");
		kengine::registerComponents<kengine::DirLightComponent>();
	}
}