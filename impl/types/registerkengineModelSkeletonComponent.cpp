#include "helpers/registerTypeHelper.hpp"
#include "data/ModelSkeletonComponent.hpp"
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine::types{
	void registerkengineModelSkeletonComponent() noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(Log, "Init/registerTypes", "Registering 'kengine::ModelSkeletonComponent'");
		kengine::registerComponents<kengine::ModelSkeletonComponent>();
	}
}