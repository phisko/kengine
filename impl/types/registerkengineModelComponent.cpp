#include "helpers/registerTypeHelper.hpp"
#include "data/ModelComponent.hpp"

namespace kengine::impl::types{
	void registerkengineModelComponent() noexcept {
		kengine::registerComponents<kengine::ModelComponent>();

	}
}