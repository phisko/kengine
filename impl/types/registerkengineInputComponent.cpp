#include "helpers/registerTypeHelper.hpp"
#include "data/InputComponent.hpp"

namespace kengine::types{
	void registerkengineInputComponent() noexcept {
		kengine::registerComponents<kengine::InputComponent>();

	}
}