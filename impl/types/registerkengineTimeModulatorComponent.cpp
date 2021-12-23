#include "helpers/registerTypeHelper.hpp"
#include "data/TimeModulatorComponent.hpp"

namespace kengine::types{
	void registerkengineTimeModulatorComponent() noexcept {
		kengine::registerComponents<kengine::TimeModulatorComponent>();

	}
}