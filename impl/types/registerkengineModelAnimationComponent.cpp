#include "helpers/registerTypeHelper.hpp"
#include "data/ModelAnimationComponent.hpp"
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine::types{
	void registerkengineModelAnimationComponent() noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(Log, "Init/registerTypes", "Registering 'kengine::ModelAnimationComponent'");
		kengine::registerComponents<kengine::ModelAnimationComponent>();
	}
}