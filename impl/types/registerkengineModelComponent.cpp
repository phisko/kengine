#include "helpers/registerTypeHelper.hpp"
#include "data/ModelComponent.hpp"
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine::types{
	void registerkengineModelComponent() noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(Log, "Init/registerTypes", "Registering 'kengine::ModelComponent'");
		kengine::registerComponents<kengine::ModelComponent>();
	}
}