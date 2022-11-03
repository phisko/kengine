#include "helpers/registerTypeHelper.hpp"
#include "data/HighlightComponent.hpp"
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine::types{
	void registerkengineHighlightComponent() noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(Log, "Init/registerTypes", "Registering 'kengine::HighlightComponent'");
		kengine::registerComponents<kengine::HighlightComponent>();
	}
}