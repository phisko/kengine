#include "helpers/registerTypeHelper.hpp"
#include "data/ViewportComponent.hpp"
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine::types{
	void registerkengineViewportComponent() noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(Log, "Init/registerTypes", "Registering 'kengine::ViewportComponent'");
		kengine::registerComponents<kengine::ViewportComponent>();
	}
}