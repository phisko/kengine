#include "helpers/registerTypeHelper.hpp"
#include "data/DebugGraphicsComponent.hpp"

namespace kengine::impl::types{
	void registerkengineDebugGraphicsComponent() noexcept {
		kengine::registerComponents<kengine::DebugGraphicsComponent>();

	}
}