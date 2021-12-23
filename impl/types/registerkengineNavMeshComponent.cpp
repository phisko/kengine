#include "helpers/registerTypeHelper.hpp"
#include "data/NavMeshComponent.hpp"

namespace kengine::types{
	void registerkengineNavMeshComponent() noexcept {
		kengine::registerComponents<kengine::NavMeshComponent>();

	}
}