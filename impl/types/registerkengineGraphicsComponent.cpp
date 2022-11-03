#include "helpers/registerTypeHelper.hpp"
#include "data/GraphicsComponent.hpp"
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine::types{
	void registerkengineGraphicsComponent() noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(Log, "Init/registerTypes", "Registering 'kengine::GraphicsComponent'");
		kengine::registerComponents<kengine::GraphicsComponent>();
	}
}