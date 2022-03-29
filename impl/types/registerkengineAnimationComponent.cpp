#include "helpers/registerTypeHelper.hpp"
#include "data/AnimationComponent.hpp"
#include "helpers/logHelper.hpp"

namespace kengine::types{
	void registerkengineAnimationComponent() noexcept {
		kengine_log(Log, "Init/registerTypes", "Registering 'kengine::AnimationComponent'");
		kengine::registerComponents<kengine::AnimationComponent>();

	}
}