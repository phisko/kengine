#include "helpers/registerTypeHelper.hpp"
#include "data/InputComponent.hpp"
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine::types{
	void registerkengineInputComponent() noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(Log, "Init/registerTypes", "Registering 'kengine::InputComponent'");
		kengine::registerComponents<kengine::InputComponent>();
	}
}