#include "helpers/registerTypeHelper.hpp"
#include "data/ModelColliderComponent.hpp"
#include "helpers/logHelper.hpp"

namespace kengine::types{
	void registerkengineModelColliderComponent() noexcept {
		kengine_log(Log, "Init/registerTypes", "Registering 'kengine::ModelColliderComponent'");
		kengine::registerComponents<kengine::ModelColliderComponent>();

	}
}