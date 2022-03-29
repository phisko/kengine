#include "helpers/registerTypeHelper.hpp"
#include "data/InputComponent.hpp"
#include "helpers/logHelper.hpp"

namespace kengine::types{
	void registerkengineInputComponent() noexcept {
		kengine_log(Log, "Init/registerTypes", "Registering 'kengine::InputComponent'");
		kengine::registerComponents<kengine::InputComponent>();

	}
}