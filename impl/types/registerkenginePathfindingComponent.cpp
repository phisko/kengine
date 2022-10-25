#include "helpers/registerTypeHelper.hpp"
#include "data/PathfindingComponent.hpp"
#include "helpers/logHelper.hpp"

namespace kengine::types{
	void registerkenginePathfindingComponent() noexcept {
		kengine_log(Log, "Init/registerTypes", "Registering 'kengine::PathfindingComponent'");
		kengine::registerComponents<kengine::PathfindingComponent>();
	}
}