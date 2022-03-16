#include "helpers/registerTypeHelper.hpp"
#include "data/CommandLineComponent.hpp"

namespace kengine::types{
	void registerkengineCommandLineComponent() noexcept {
		kengine::registerComponents<kengine::CommandLineComponent>();

	}
}