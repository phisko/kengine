#include "helpers/registerTypeHelper.hpp"
#include "data/PathfindingComponent.hpp"

namespace kengine::impl::types{
	void registerkenginePathfindingComponent() noexcept {
		kengine::registerComponents<kengine::PathfindingComponent>();

	}
}