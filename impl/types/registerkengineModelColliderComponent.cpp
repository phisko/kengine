#include "helpers/registerTypeHelper.hpp"
#include "data/ModelColliderComponent.hpp"

namespace kengine::types{
	void registerkengineModelColliderComponent() noexcept {
		kengine::registerComponents<kengine::ModelColliderComponent>();

	}
}