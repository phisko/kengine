#include "helpers/registerTypeHelper.hpp"
#include "data/AdjustableComponent.hpp"

namespace kengine::types{
	void registerkengineAdjustableComponent() noexcept {
		kengine::registerComponents<kengine::AdjustableComponent>();

	}
}