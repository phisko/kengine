#include "helpers/registerTypeHelper.hpp"
#include "data/TimeModulatorComponent.hpp"
#include "helpers/logHelper.hpp"

namespace kengine::types{
	void registerkengineTimeModulatorComponent() noexcept {
		kengine_log(Log, "Init/registerTypes", "Registering 'kengine::TimeModulatorComponent'");
		kengine::registerComponents<kengine::TimeModulatorComponent>();

	}
}