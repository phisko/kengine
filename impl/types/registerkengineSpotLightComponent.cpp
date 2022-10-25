#include "helpers/registerTypeHelper.hpp"
#include "data/LightComponent.hpp"
#include "helpers/logHelper.hpp"

namespace kengine::types{
	void registerkengineSpotLightComponent() noexcept {
		kengine_log(Log, "Init/registerTypes", "Registering 'kengine::SpotLightComponent'");
		kengine::registerComponents<kengine::SpotLightComponent>();
	}
}