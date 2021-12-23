#include "helpers/registerTypeHelper.hpp"
#include "data/ModelComponent.hpp"

namespace kengine::types{
	void registerkengineModelComponent() noexcept {
		kengine::registerComponents<kengine::ModelComponent>();

	}
}