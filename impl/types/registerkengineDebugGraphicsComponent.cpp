#include "helpers/registerTypeHelper.hpp"
#include "data/DebugGraphicsComponent.hpp"
#include "helpers/logHelper.hpp"

namespace kengine::types{
	void registerkengineDebugGraphicsComponent() noexcept {
		kengine_log(Log, "Init/registerTypes", "Registering 'kengine::DebugGraphicsComponent'");
		kengine::registerComponents<kengine::DebugGraphicsComponent>();
	}
}