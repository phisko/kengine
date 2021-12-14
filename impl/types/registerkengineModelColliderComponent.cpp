#include "helpers/registerTypeHelper.hpp"
#include "data/ModelColliderComponent.hpp"

namespace kengine::impl::types{
	void registerkengineModelColliderComponent() noexcept {
		kengine::registerComponents<kengine::ModelColliderComponent>();

	}
}