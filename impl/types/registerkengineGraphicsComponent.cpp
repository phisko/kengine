#include "helpers/registerTypeHelper.hpp"
#include "data/GraphicsComponent.hpp"

namespace kengine::impl::types{
	void registerkengineGraphicsComponent() noexcept {
		kengine::registerComponents<kengine::GraphicsComponent>();

	}
}