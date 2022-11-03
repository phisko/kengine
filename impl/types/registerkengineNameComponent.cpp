#include "helpers/registerTypeHelper.hpp"
#include "data/NameComponent.hpp"
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine::types{
	void registerkengineNameComponent() noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(Log, "Init/registerTypes", "Registering 'kengine::NameComponent'");
		kengine::registerComponents<kengine::NameComponent>();
	}
}