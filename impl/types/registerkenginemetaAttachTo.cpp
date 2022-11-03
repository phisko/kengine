#include "helpers/registerTypeHelper.hpp"
#include "meta/AttachTo.hpp"
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine::types{
	void registerkenginemetaAttachTo() noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(Log, "Init/registerTypes", "Registering 'kengine::meta::AttachTo'");
		kengine::registerComponents<kengine::meta::AttachTo>();
	}
}