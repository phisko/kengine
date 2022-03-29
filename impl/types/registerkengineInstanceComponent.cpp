#include "helpers/registerTypeHelper.hpp"
#include "data/InstanceComponent.hpp"
#include "helpers/logHelper.hpp"

namespace kengine::types{
	void registerkengineInstanceComponent() noexcept {
		kengine_log(Log, "Init/registerTypes", "Registering 'kengine::InstanceComponent'");
		kengine::registerComponents<kengine::InstanceComponent>();

	}
}