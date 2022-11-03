#include "helpers/registerTypeHelper.hpp"
#include "data/PathfindingComponent.hpp"
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine::types{
	void registerkenginePathfindingComponent() noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(Log, "Init/registerTypes", "Registering 'kengine::PathfindingComponent'");
		kengine::registerComponents<kengine::PathfindingComponent>();
	}
}