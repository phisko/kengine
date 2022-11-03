#include "helpers/registerTypeHelper.hpp"
#include "data/ModelColliderComponent.hpp"
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine::types{
	void registerkengineModelColliderComponent() noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(Log, "Init/registerTypes", "Registering 'kengine::ModelColliderComponent'");
		kengine::registerComponents<kengine::ModelColliderComponent>();
	}
}