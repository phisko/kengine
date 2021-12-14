#include "helpers/registerTypeHelper.hpp"
#include "data/AnimationComponent.hpp"

namespace kengine::impl::types{
	void registerkengineAnimationComponent() noexcept {
		kengine::registerComponents<kengine::AnimationComponent>();

	}
}