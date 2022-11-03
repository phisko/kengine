#include "helpers/registerTypeHelper.hpp"
#include "data/GodRaysComponent.hpp"
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine::types{
	void registerkengineGodRaysComponent() noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(Log, "Init/registerTypes", "Registering 'kengine::GodRaysComponent'");
		kengine::registerComponents<kengine::GodRaysComponent>();
	}
}