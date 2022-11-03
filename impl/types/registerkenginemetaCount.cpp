#include "helpers/registerTypeHelper.hpp"
#include "meta/Count.hpp"
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine::types{
	void registerkenginemetaCount() noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(Log, "Init/registerTypes", "Registering 'kengine::meta::Count'");
		kengine::registerComponents<kengine::meta::Count>();
	}
}