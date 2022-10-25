#include "helpers/registerTypeHelper.hpp"
#include "data/NavMeshComponent.hpp"
#include "helpers/logHelper.hpp"

namespace kengine::types{
	void registerkengineNavMeshComponent() noexcept {
		kengine_log(Log, "Init/registerTypes", "Registering 'kengine::NavMeshComponent'");
		kengine::registerComponents<kengine::NavMeshComponent>();
	}
}