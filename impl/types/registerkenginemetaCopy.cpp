#include "helpers/registerTypeHelper.hpp"
#include "meta/Copy.hpp"
#include "helpers/logHelper.hpp"

namespace kengine::types{
	void registerkenginemetaCopy() noexcept {
		kengine_log(Log, "Init/registerTypes", "Registering 'kengine::meta::Copy'");
		kengine::registerComponents<kengine::meta::Copy>();

	}
}