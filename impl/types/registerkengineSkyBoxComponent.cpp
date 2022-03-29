#include "helpers/registerTypeHelper.hpp"
#include "data/SkyBoxComponent.hpp"
#include "helpers/logHelper.hpp"

namespace kengine::types{
	void registerkengineSkyBoxComponent() noexcept {
		kengine_log(Log, "Init/registerTypes", "Registering 'kengine::SkyBoxComponent'");
		kengine::registerComponents<kengine::SkyBoxComponent>();

	}
}