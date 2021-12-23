#include "helpers/registerTypeHelper.hpp"
#include "data/LightComponent.hpp"

namespace kengine::types{
	void registerkengineSpotLightComponent() noexcept {
		kengine::registerComponents<kengine::SpotLightComponent>();

	}
}