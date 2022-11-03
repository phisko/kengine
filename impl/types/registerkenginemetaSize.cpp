#include "helpers/registerTypeHelper.hpp"
#include "meta/Size.hpp"
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine::types{
	void registerkenginemetaSize() noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(Log, "Init/registerTypes", "Registering 'kengine::meta::Size'");
		kengine::registerComponents<kengine::meta::Size>();
	}
}