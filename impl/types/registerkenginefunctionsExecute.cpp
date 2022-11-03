#include "helpers/registerTypeHelper.hpp"
#include "functions/Execute.hpp"
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine::types{
	void registerkenginefunctionsExecute() noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(Log, "Init/registerTypes", "Registering 'kengine::functions::Execute'");
		kengine::registerComponents<kengine::functions::Execute>();
	}
}