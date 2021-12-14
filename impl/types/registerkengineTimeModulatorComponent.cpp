#include "helpers/registerTypeHelper.hpp"
#include "data/TimeModulatorComponent.hpp"

namespace kengine::impl::types{
	void registerkengineTimeModulatorComponent() noexcept {
		kengine::registerComponents<kengine::TimeModulatorComponent>();

	}
}