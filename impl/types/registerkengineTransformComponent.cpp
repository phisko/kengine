#include "helpers/registerTypeHelper.hpp"
#include "data/TransformComponent.hpp"
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine::types{
	void registerkengineTransformComponent() noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(Log, "Init/registerTypes", "Registering 'kengine::TransformComponent'");
		kengine::registerComponents<kengine::TransformComponent>();
	}
}