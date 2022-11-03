#include "helpers/registerTypeHelper.hpp"
#include "data/InstanceComponent.hpp"
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine::types{
	void registerkengineInstanceComponent() noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(Log, "Init/registerTypes", "Registering 'kengine::InstanceComponent'");
		kengine::registerComponents<kengine::InstanceComponent>();
	}
}