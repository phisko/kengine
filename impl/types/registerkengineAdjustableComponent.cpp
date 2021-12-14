#include "helpers/registerTypeHelper.hpp"
#include "data/AdjustableComponent.hpp"

namespace kengine::impl::types{
	void registerkengineAdjustableComponent() noexcept {
		kengine::registerComponents<kengine::AdjustableComponent>();

	}
}