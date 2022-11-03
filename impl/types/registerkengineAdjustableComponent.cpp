#include "helpers/registerTypeHelper.hpp"
#include "data/AdjustableComponent.hpp"
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine::types{
	void registerkengineAdjustableComponent() noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(Log, "Init/registerTypes", "Registering 'kengine::AdjustableComponent'");
		kengine::registerComponents<kengine::AdjustableComponent>();
	}
}