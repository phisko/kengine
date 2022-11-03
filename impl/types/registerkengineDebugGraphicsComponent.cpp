#include "helpers/registerTypeHelper.hpp"
#include "data/DebugGraphicsComponent.hpp"
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine::types{
	void registerkengineDebugGraphicsComponent() noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(Log, "Init/registerTypes", "Registering 'kengine::DebugGraphicsComponent'");
		kengine::registerComponents<kengine::DebugGraphicsComponent>();
	}
}