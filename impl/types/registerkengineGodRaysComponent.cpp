#include "helpers/registerTypeHelper.hpp"
#include "data/GodRaysComponent.hpp"
#include "helpers/logHelper.hpp"

namespace kengine::types{
	void registerkengineGodRaysComponent() noexcept {
		kengine_log(Log, "Init/registerTypes", "Registering 'kengine::GodRaysComponent'");
		kengine::registerComponents<kengine::GodRaysComponent>();

	}
}