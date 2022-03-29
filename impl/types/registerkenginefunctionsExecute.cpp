#include "helpers/registerTypeHelper.hpp"
#include "functions/Execute.hpp"
#include "helpers/logHelper.hpp"

namespace kengine::types{
	void registerkenginefunctionsExecute() noexcept {
		kengine_log(Log, "Init/registerTypes", "Registering 'kengine::functions::Execute'");
		kengine::registerComponents<kengine::functions::Execute>();

	}
}