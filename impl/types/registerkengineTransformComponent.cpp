#include "helpers/registerTypeHelper.hpp"
#include "data/TransformComponent.hpp"

namespace kengine::types{
	void registerkengineTransformComponent() noexcept {
		kengine::registerComponents<kengine::TransformComponent>();

	}
}