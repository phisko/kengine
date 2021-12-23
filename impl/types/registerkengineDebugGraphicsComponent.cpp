#include "helpers/registerTypeHelper.hpp"
#include "data/DebugGraphicsComponent.hpp"

namespace kengine::types{
	void registerkengineDebugGraphicsComponent() noexcept {
		kengine::registerComponents<kengine::DebugGraphicsComponent>();

	}
}