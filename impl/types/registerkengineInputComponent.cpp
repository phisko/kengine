#include "helpers/registerTypeHelper.hpp"
#include "data/InputComponent.hpp"

namespace kengine::impl::types{
	void registerkengineInputComponent() noexcept {
		kengine::registerComponents<kengine::InputComponent>();

	}
}