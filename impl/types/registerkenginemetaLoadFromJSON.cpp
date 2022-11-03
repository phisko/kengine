#include "helpers/registerTypeHelper.hpp"
#include "meta/LoadFromJSON.hpp"
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine::types{
	void registerkenginemetaLoadFromJSON() noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(Log, "Init/registerTypes", "Registering 'kengine::meta::LoadFromJSON'");
		kengine::registerComponents<kengine::meta::LoadFromJSON>();
	}
}