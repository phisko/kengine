#include "helpers/registerTypeHelper.hpp"
#include "meta/Has.hpp"
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine::types{
	void registerkenginemetaHas() noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(Log, "Init/registerTypes", "Registering 'kengine::meta::Has'");
		kengine::registerComponents<kengine::meta::Has>();
	}
}