#include "helpers/registerTypeHelper.hpp"
#include "data/SkyBoxComponent.hpp"
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine::types{
	void registerkengineSkyBoxComponent() noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(Log, "Init/registerTypes", "Registering 'kengine::SkyBoxComponent'");
		kengine::registerComponents<kengine::SkyBoxComponent>();
	}
}