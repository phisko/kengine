#include "helpers/registerTypeHelper.hpp"
#include "meta/Attributes.hpp"
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine::types{
	void registerkenginemetaAttributes() noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(Log, "Init/registerTypes", "Registering 'kengine::meta::Attributes'");
		kengine::registerComponents<kengine::meta::Attributes>();
	}
}