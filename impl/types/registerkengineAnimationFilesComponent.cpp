#include "helpers/registerTypeHelper.hpp"
#include "data/AnimationFilesComponent.hpp"
#include "helpers/logHelper.hpp"

namespace kengine::types{
	void registerkengineAnimationFilesComponent() noexcept {
		kengine_log(Log, "Init/registerTypes", "Registering 'kengine::AnimationFilesComponent'");
		kengine::registerComponents<kengine::AnimationFilesComponent>();
	}
}