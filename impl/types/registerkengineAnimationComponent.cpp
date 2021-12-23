#include "helpers/registerTypeHelper.hpp"
#include "data/AnimationComponent.hpp"

namespace kengine::types{
	void registerkengineAnimationComponent() noexcept {
		kengine::registerComponents<kengine::AnimationComponent>();

	}
}