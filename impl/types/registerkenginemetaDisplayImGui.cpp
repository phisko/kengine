#include "helpers/registerTypeHelper.hpp"
#include "meta/DisplayImGui.hpp"
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine::types{
	void registerkenginemetaDisplayImGui() noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(Log, "Init/registerTypes", "Registering 'kengine::meta::DisplayImGui'");
		kengine::registerComponents<kengine::meta::DisplayImGui>();
	}
}