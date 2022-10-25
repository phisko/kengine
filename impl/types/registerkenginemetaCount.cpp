#include "helpers/registerTypeHelper.hpp"
#include "meta/Count.hpp"
#include "helpers/logHelper.hpp"

namespace kengine::types{
	void registerkenginemetaCount() noexcept {
		kengine_log(Log, "Init/registerTypes", "Registering 'kengine::meta::Count'");
		kengine::registerComponents<kengine::meta::Count>();
	}
}