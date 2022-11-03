#include "helpers/registerTypeHelper.hpp"
#include "functions/AppearsInViewport.hpp"
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine::types{
	void registerkenginefunctionsAppearsInViewport() noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(Log, "Init/registerTypes", "Registering 'kengine::functions::AppearsInViewport'");
		kengine::registerComponents<kengine::functions::AppearsInViewport>();
	}
}