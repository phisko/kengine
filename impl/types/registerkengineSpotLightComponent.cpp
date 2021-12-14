#include "helpers/registerTypeHelper.hpp"
#include "data/LightComponent.hpp"

namespace kengine::impl::types{
	void registerkengineSpotLightComponent() noexcept {
		kengine::registerComponents<kengine::SpotLightComponent>();

	}
}