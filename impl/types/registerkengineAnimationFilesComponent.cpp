#include "helpers/registerTypeHelper.hpp"
#include "data/AnimationFilesComponent.hpp"

namespace kengine::impl::types{
	void registerkengineAnimationFilesComponent() noexcept {
		kengine::registerComponents<kengine::AnimationFilesComponent>();

	}
}