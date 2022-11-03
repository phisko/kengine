#include "helpers/registerTypeHelper.hpp"
#include "data/AnimationFilesComponent.hpp"
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine::types{
	void registerkengineAnimationFilesComponent() noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(Log, "Init/registerTypes", "Registering 'kengine::AnimationFilesComponent'");
		kengine::registerComponents<kengine::AnimationFilesComponent>();
	}
}