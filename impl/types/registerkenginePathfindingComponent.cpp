#include "helpers/registerTypeHelper.hpp"
#include "data/PathfindingComponent.hpp"

namespace kengine::types{
	void registerkenginePathfindingComponent() noexcept {
		kengine::registerComponents<kengine::PathfindingComponent>();

	}
}