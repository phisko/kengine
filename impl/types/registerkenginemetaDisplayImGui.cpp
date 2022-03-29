#include "helpers/registerTypeHelper.hpp"
#include "meta/DisplayImGui.hpp"
#include "helpers/logHelper.hpp"

namespace kengine::types{
	void registerkenginemetaDisplayImGui() noexcept {
		kengine_log(Log, "Init/registerTypes", "Registering 'kengine::meta::DisplayImGui'");
		kengine::registerComponents<kengine::meta::DisplayImGui>();

	}
}