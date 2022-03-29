#include "helpers/registerTypeHelper.hpp"
#include "meta/EditImGui.hpp"
#include "helpers/logHelper.hpp"

namespace kengine::types{
	void registerkenginemetaEditImGui() noexcept {
		kengine_log(Log, "Init/registerTypes", "Registering 'kengine::meta::EditImGui'");
		kengine::registerComponents<kengine::meta::EditImGui>();

	}
}