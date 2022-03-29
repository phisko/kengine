#include "helpers/registerTypeHelper.hpp"
#include "data/ModelComponent.hpp"
#include "helpers/logHelper.hpp"

namespace kengine::types{
	void registerkengineModelComponent() noexcept {
		kengine_log(Log, "Init/registerTypes", "Registering 'kengine::ModelComponent'");
		kengine::registerComponents<kengine::ModelComponent>();

	}
}