#include "helpers/registerTypeHelper.hpp"
#include "data/TransformComponent.hpp"

namespace kengine::impl::types{
	void registerkengineTransformComponent() noexcept {
		kengine::registerComponents<kengine::TransformComponent>();

	}
}