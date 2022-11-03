#include "helpers/registerTypeHelper.hpp"
#include "data/NavMeshComponent.hpp"
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine::types{
	void registerkengineNavMeshComponent() noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(Log, "Init/registerTypes", "Registering 'kengine::NavMeshComponent'");
		kengine::registerComponents<kengine::NavMeshComponent>();
	}
}