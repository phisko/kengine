#include "helpers/registerTypeHelper.hpp"
#include "data/GraphicsComponent.hpp"

namespace kengine::types{
	void registerkengineGraphicsComponent() noexcept {
		kengine::registerComponents<kengine::GraphicsComponent>();

	}
}