#include "helpers/registerTypeHelper.hpp"
#include "meta/Has.hpp"
#include "helpers/logHelper.hpp"

namespace kengine::types{
	void registerkenginemetaHas() noexcept {
		kengine_log(Log, "Init/registerTypes", "Registering 'kengine::meta::Has'");
		kengine::registerComponents<kengine::meta::Has>();
	}
}