#include "helpers/registerTypeHelper.hpp"
#include "meta/LoadFromJSON.hpp"
#include "helpers/logHelper.hpp"

namespace kengine::types{
	void registerkenginemetaLoadFromJSON() noexcept {
		kengine_log(Log, "Init/registerTypes", "Registering 'kengine::meta::LoadFromJSON'");
		kengine::registerComponents<kengine::meta::LoadFromJSON>();

	}
}