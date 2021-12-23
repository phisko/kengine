#include "helpers/registerTypeHelper.hpp"
#include "data/LightComponent.hpp"

namespace kengine::types{
	void registerkenginePointLightComponent() noexcept {
		kengine::registerComponents<kengine::PointLightComponent>();

	}
}