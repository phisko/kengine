#include "helpers/registerTypeHelper.hpp"
#include "data/TimeModulatorComponent.hpp"
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine::types{
	void registerkengineTimeModulatorComponent() noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(Log, "Init/registerTypes", "Registering 'kengine::TimeModulatorComponent'");
		kengine::registerComponents<kengine::TimeModulatorComponent>();
	}
}