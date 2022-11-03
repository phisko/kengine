#include "helpers/registerTypeHelper.hpp"
#include "meta/DetachFrom.hpp"
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine::types{
	void registerkenginemetaDetachFrom() noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(Log, "Init/registerTypes", "Registering 'kengine::meta::DetachFrom'");
		kengine::registerComponents<kengine::meta::DetachFrom>();
	}
}