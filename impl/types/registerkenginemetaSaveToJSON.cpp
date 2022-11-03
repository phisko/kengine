#include "helpers/registerTypeHelper.hpp"
#include "meta/SaveToJSON.hpp"
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine::types{
	void registerkenginemetaSaveToJSON() noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(Log, "Init/registerTypes", "Registering 'kengine::meta::SaveToJSON'");
		kengine::registerComponents<kengine::meta::SaveToJSON>();
	}
}