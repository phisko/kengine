#include "helpers/registerTypeHelper.hpp"
#include "data/LightComponent.hpp"
#include "helpers/logHelper.hpp"

namespace kengine::types{
	void registerkenginePointLightComponent() noexcept {
		kengine_log(Log, "Init/registerTypes", "Registering 'kengine::PointLightComponent'");
		kengine::registerComponents<kengine::PointLightComponent>();
	}
}