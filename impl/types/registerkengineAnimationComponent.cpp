#include "helpers/registerTypeHelper.hpp"
#include "data/AnimationComponent.hpp"
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine::types{
	void registerkengineAnimationComponent() noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(Log, "Init/registerTypes", "Registering 'kengine::AnimationComponent'");
		kengine::registerComponents<kengine::AnimationComponent>();
	}
}