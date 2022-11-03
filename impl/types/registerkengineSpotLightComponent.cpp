#include "helpers/registerTypeHelper.hpp"
#include "data/LightComponent.hpp"
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine::types{
	void registerkengineSpotLightComponent() noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(Log, "Init/registerTypes", "Registering 'kengine::SpotLightComponent'");
		kengine::registerComponents<kengine::SpotLightComponent>();
	}
}