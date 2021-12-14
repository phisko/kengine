#include "helpers/registerTypeHelper.hpp"
#include "data/ModelAnimationComponent.hpp"

namespace kengine::impl::types{
	void registerkengineModelAnimationComponent() noexcept {
		kengine::registerComponents<kengine::ModelAnimationComponent>();

	}
}