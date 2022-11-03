#include "helpers/registerTypeHelper.hpp"
#include "data/SelectedComponent.hpp"
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine::types{
	void registerkengineSelectedComponent() noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(Log, "Init/registerTypes", "Registering 'kengine::SelectedComponent'");
		kengine::registerComponents<kengine::SelectedComponent>();
	}
}