#include "helpers/registerTypeHelper.hpp"
#include "meta/ForEachEntity.hpp"
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine::types{
	void registerkenginemetaForEachEntityWithout() noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(Log, "Init/registerTypes", "Registering 'kengine::meta::ForEachEntityWithout'");
		kengine::registerComponents<kengine::meta::ForEachEntityWithout>();
	}
}