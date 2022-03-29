#include "helpers/registerTypeHelper.hpp"
#include "data/AdjustableComponent.hpp"
#include "helpers/logHelper.hpp"

namespace kengine::types{
	void registerkengineAdjustableComponent() noexcept {
		kengine_log(Log, "Init/registerTypes", "Registering 'kengine::AdjustableComponent'");
		kengine::registerComponents<kengine::AdjustableComponent>();

	}
}