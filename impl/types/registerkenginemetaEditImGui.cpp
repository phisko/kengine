#include "helpers/registerTypeHelper.hpp"
#include "meta/EditImGui.hpp"
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine::types{
	void registerkenginemetaEditImGui() noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(Log, "Init/registerTypes", "Registering 'kengine::meta::EditImGui'");
		kengine::registerComponents<kengine::meta::EditImGui>();
	}
}