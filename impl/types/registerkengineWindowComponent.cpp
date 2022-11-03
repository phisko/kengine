#include "helpers/registerTypeHelper.hpp"
#include "data/WindowComponent.hpp"
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine::types{
	void registerkengineWindowComponent() noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(Log, "Init/registerTypes", "Registering 'kengine::WindowComponent'");
		kengine::registerComponents<kengine::WindowComponent>();
	}
}