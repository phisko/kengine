#include "helpers/registerTypeHelper.hpp"
#include "data/InstanceComponent.hpp"

namespace kengine::impl::types{
	void registerkengineInstanceComponent() noexcept {
		kengine::registerComponents<kengine::InstanceComponent>();

	}
}