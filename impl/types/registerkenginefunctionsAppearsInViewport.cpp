#include "helpers/registerTypeHelper.hpp"
#include "functions/AppearsInViewport.hpp"
#include "helpers/logHelper.hpp"

namespace kengine::types{
	void registerkenginefunctionsAppearsInViewport() noexcept {
		kengine_log(Log, "Init/registerTypes", "Registering 'kengine::functions::AppearsInViewport'");
		kengine::registerComponents<kengine::functions::AppearsInViewport>();

	}
}